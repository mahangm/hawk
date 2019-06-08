#include "../system/core.hpp"
#include "camera.hpp"
#include "../objects/entity.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace hawk
{
	namespace Components
	{
		Camera::Camera() : onPreRender(nullptr), onPostRender(nullptr), projection(Projection::Perspective), background(0.0f), skybox(nullptr),
			irradiance(nullptr), radiance(nullptr), fov(45.0f), near_plane(0.1f), far_plane(100.0f), 
			front(glm::vec3(0.0f, 0.0f, -1.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)), right(glm::vec3(1.0f, 0.0f, 0.0f)),
			m_default_framebuffer(nullptr), m_deferred_framebuffer(nullptr), m_screen_framebuffer(nullptr), m_final_framebuffer(nullptr)
		{
			// set component type
			m_type = "Camera";

			// application
			System::Core& app = System::Core::getInstance();

			// set viewport
			viewport = glm::vec4(0, 0, app.width, app.height);
		}

		Camera::Camera(const Camera& other) : Component(other), onPreRender(other.onPreRender), onPostRender(other.onPostRender), projection(other.projection),
			background(other.background), skybox(other.skybox), viewport(other.viewport), materials(other.materials), 
			fov(other.fov), near_plane(other.near_plane), far_plane(other.far_plane), front(other.front), up(other.up), right(other.right), 
			m_default_framebuffer(other.m_default_framebuffer), m_deferred_framebuffer(other.m_deferred_framebuffer), m_screen_framebuffer(other.m_screen_framebuffer),
			m_final_framebuffer(other.m_final_framebuffer)
		{
			// set component type
			m_type = "Camera";
		}

		Camera::Camera(Camera::Projection projection, const glm::vec4& background, const glm::ivec4& viewport) : onPreRender(nullptr), onPostRender(nullptr),
			projection(projection), background(background), skybox(nullptr), viewport(viewport), fov(45.0f), near_plane(0.1f), far_plane(100.0f),
			front(glm::vec3(0.0f, 0.0f, -1.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)), right(glm::vec3(1.0f, 0.0f, 0.0f)), 
			m_default_framebuffer(nullptr), m_deferred_framebuffer(nullptr), m_screen_framebuffer(nullptr), m_final_framebuffer(nullptr)
		{
			// set component type
			m_type = "Camera";
		}

		Camera::~Camera() 
		{
			// application
			System::Core& app = System::Core::getInstance();

			// reset video camera
			if (app.video.camera == this) app.video.camera = nullptr;

			// remove camera from render
			removeFromVideoCameras();
		}

		void Camera::operator=(const Camera& other)
		{
			onPreRender = other.onPreRender;
			onPostRender = other.onPostRender;
			projection = other.projection;
			background = other.background;
			skybox = other.skybox;
			viewport = other.viewport;
			materials = other.materials;
			fov = other.fov;
			near_plane = other.near_plane;
			far_plane = other.far_plane;
			front = other.front;
			up = other.up;
			right = other.right;
			m_default_framebuffer = other.m_default_framebuffer;
			m_deferred_framebuffer = other.m_deferred_framebuffer;
			m_screen_framebuffer = other.m_screen_framebuffer;
			m_final_framebuffer = other.m_final_framebuffer;
		}

		Camera* Camera::copy()
		{
			return new Camera(*this);
		}

		void Camera::sendMessage(const char* message)
		{
			// return if not enable
			if (!enable) return;

			// events
			if (std::strcmp(message, "Start") == 0)
				start();
			else if (std::strcmp(message, "Update") == 0)
				update();
			else if (std::strcmp(message, "PreRender") == 0)
				preRender();
			else if (std::strcmp(message, "PostRender") == 0)
				postRender();
			else if (std::strcmp(message, "Destroy") == 0)
				destroy();
		}

		void Camera::release()
		{
			// free default frame buffer
			if (m_default_framebuffer)
			{
				m_default_framebuffer->release();

				m_default_framebuffer = nullptr;
			}

			// free deferred frame buffer
			if (m_deferred_framebuffer)
			{
				m_deferred_framebuffer->release();

				m_deferred_framebuffer = nullptr;
			}

			// free screen frame buffer
			if (m_screen_framebuffer)
			{
				m_screen_framebuffer->release();

				m_screen_framebuffer = nullptr;
			}

			// free final frame buffer
			if (m_final_framebuffer)
			{
				m_final_framebuffer->release();

				m_final_framebuffer = nullptr;
			}

			// free object
			delete this;
		}
		
		void Camera::start()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// make default frame buffer
			m_default_framebuffer = new Assets::Framebuffer();

			if (!m_default_framebuffer->build(viewport.z, viewport.w, GL_TEXTURE_2D,
				GL_CLAMP_TO_EDGE, GL_LINEAR, glm::vec4(1.0f), 
				Assets::Framebuffer::Attachment::Color | Assets::Framebuffer::Attachment::Depth | Assets::Framebuffer::Attachment::Stencil, 
				std::vector<Assets::Framebuffer::BufferFormat> { { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT } }))
			{
				app.logger.error("Failed to create camera default frame buffer!");
				app.logger.info(m_name.c_str());

				// release frame buffer
				m_default_framebuffer->release();

				m_default_framebuffer = nullptr;
			}

			// make deferred frame buffer (g-buffer)
			m_deferred_framebuffer = new Assets::Framebuffer();

			if (!m_deferred_framebuffer->build(viewport.z, viewport.w, GL_TEXTURE_2D,
				GL_CLAMP_TO_EDGE, GL_NEAREST, glm::vec4(1.0f),
				Assets::Framebuffer::Attachment::Color | Assets::Framebuffer::Attachment::Depth | Assets::Framebuffer::Attachment::Stencil, 
				std::vector<Assets::Framebuffer::BufferFormat> { 
					{ GL_RGB16F, GL_RGB, GL_HALF_FLOAT }, // position
					{ GL_RGB16F, GL_RGB, GL_HALF_FLOAT }, // normal
					{ GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE }, // diffuse / albedo
					{ GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT }, // specular + shininess / metallic + roughness + ambient + shadows
					{ GL_RG16F, GL_RG, GL_HALF_FLOAT } // extra
				}))
			{
				app.logger.error("Failed to create camera deferred frame buffer!");
				app.logger.info(m_name.c_str());

				// release frame buffer
				m_deferred_framebuffer->release();

				m_deferred_framebuffer = nullptr;
			}

			// make screen frame buffer
			m_screen_framebuffer = new Assets::Framebuffer();

			if (!m_screen_framebuffer->build(viewport.z, viewport.w, GL_TEXTURE_2D,
				GL_CLAMP_TO_EDGE, GL_LINEAR, glm::vec4(1.0f),
				Assets::Framebuffer::Attachment::Color,
				std::vector<Assets::Framebuffer::BufferFormat> { { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT }, { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT } }))
			{
				app.logger.error("Failed to create camera screen frame buffer!");
				app.logger.info(m_name.c_str());

				// release frame buffer
				m_screen_framebuffer->release();

				m_screen_framebuffer = nullptr;
			}

			// make final frame buffer
			m_final_framebuffer = new Assets::Framebuffer();

			if (!m_final_framebuffer->build(viewport.z, viewport.w, GL_TEXTURE_2D,
				GL_CLAMP_TO_EDGE, GL_LINEAR, glm::vec4(1.0f),
				Assets::Framebuffer::Attachment::Color,
				std::vector<Assets::Framebuffer::BufferFormat> { { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT } }))
			{
				app.logger.error("Failed to create camera final frame buffer!");
				app.logger.info(m_name.c_str());

				// release frame buffer
				m_final_framebuffer->release();

				m_final_framebuffer = nullptr;
			}

			// calculate vectors
			calculate();

			// add camera to be rendered
			addToVideoCameras();

			// set video camera if empty
			if (!app.video.camera) app.video.camera = this;
		}

		void Camera::update()
		{
			// calculate vectors
			calculate();
		}

		void Camera::preRender()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// bind frame buffer
			switch (app.video.getRendering())
			{
				case Managers::Video::Rendering::Forward:
					// bind frame buffer
					m_default_framebuffer->bind();

					// clear frame buffer
					m_default_framebuffer->clear(background);
					break;
				case Managers::Video::Rendering::Deferred:
					// bind frame buffer
					m_deferred_framebuffer->bind();

					// clear deferred buffer with zeroes
					m_deferred_framebuffer->clear(glm::vec4(0.0f));
					break;
			}

			// raise pre-render event
			if (onPreRender) (*onPreRender)(this);
		}

		void Camera::postRender()
		{
			// unbind frame buffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// raise post-render event
			if (onPostRender) (*onPostRender)(this);
		}

		void Camera::destroy()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// remove camera from render
			removeFromVideoCameras();

			// reset video camera
			if (app.video.camera == this) app.video.camera = nullptr;
		}

		void Camera::fillCameraBuffer(unsigned int ubo)
		{
			// camera view matrix
			glm::mat4 view = glm::mat4(1.0f);

			// camera projection matrix
			glm::mat4 projection = glm::mat4(1.0f);

			// camera position vector
			glm::vec4 position = glm::vec4(entity->transform->getWorldPosition(), 0.0f);

			// get camera matrices
			view = getViewMatrix();
			projection = getProjectionMatrix();

			// image based lighting flag
			int ibl = (irradiance && radiance) ? 1 : 0;

			// fill camera ubo
			glBindBuffer(GL_UNIFORM_BUFFER, ubo);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, 64, glm::value_ptr(view));
			glBufferSubData(GL_UNIFORM_BUFFER, 64, 64, glm::value_ptr(projection));
			glBufferSubData(GL_UNIFORM_BUFFER, 128, 16, glm::value_ptr(position));
			glBufferSubData(GL_UNIFORM_BUFFER, 144, 16, glm::value_ptr(glm::vec4(viewport)));
			glBufferSubData(GL_UNIFORM_BUFFER, 160, 4, &ibl);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		glm::mat4 Camera::getViewMatrix() const
		{
			return glm::lookAt(entity->transform->getWorldPosition(), entity->transform->getWorldPosition() + front, up);
		}

		glm::mat4 Camera::getProjectionMatrix() const
		{
			// application
			System::Core& app = System::Core::getInstance();

			if (projection == Projection::Orthographic)
				return glm::ortho(0.0f, (float)app.width, 0.0f, (float)app.height, near_plane, far_plane);
			else
				return glm::perspective(glm::radians(fov), (float)app.width / (float)app.height, near_plane, far_plane);
		}

		Assets::Framebuffer* Camera::getDefaultFramebuffer() const
		{
			return m_default_framebuffer;
		}

		Assets::Framebuffer* Camera::getDeferredFramebuffer() const
		{
			return m_deferred_framebuffer;
		}

		Assets::Framebuffer* Camera::getScreenFramebuffer() const
		{
			return m_screen_framebuffer;
		}

		Assets::Framebuffer* Camera::getFinalFramebuffer() const
		{
			return m_final_framebuffer;
		}

		void Camera::calculate()
		{
			// rotation in world space
			glm::vec3 world_rotation = entity->transform->getWorldRotation();

			// set initial -90.0 degrees for euler angles
			world_rotation.x = -90.0f + world_rotation.x;

			// camera front vector
			glm::vec3 direction(std::cos(glm::radians(world_rotation.x)) * std::cos(glm::radians(world_rotation.y)),
				std::sin(glm::radians(world_rotation.y)),
				std::sin(glm::radians(world_rotation.x)) * std::cos(glm::radians(world_rotation.y)));

			// normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement
			front = glm::normalize(direction);
			right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
			up = glm::normalize(glm::cross(right, front));
		}

		void Camera::addToVideoCameras()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// add to cameras list
			app.video.cameras.push_back(this);
		}

		void Camera::removeFromVideoCameras()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// camera iterator
			auto iterator = std::find(app.video.cameras.begin(), app.video.cameras.end(), this);

			// remove from cameras list
			if (iterator != app.video.cameras.end())
				app.video.cameras.erase(iterator);
		}
	}
}