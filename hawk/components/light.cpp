#include "../system/core.hpp"
#include "light.hpp"
#include "../objects/entity.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace hawk
{
	namespace Components
	{
		Light::Light() : color(1.0f), intensity(1.0f), range(10.0f),
			cut_off(glm::cos(glm::radians(12.5f))), outer_cut_off(glm::cos(glm::radians(17.5f))), cast_shadows(false),
			frustum(-10.0f, 10.0f, -10.0f, 10.0f), near_plane(1.0f), far_plane(25.0f), m_light_type(Type::Point), m_shadow_map_frame_buffer(nullptr)
		{
			// set component type
			m_type = "Light";
		}

		Light::Light(const Light& other) : Component(other), color(other.color), intensity(other.intensity), range(other.range),
			cut_off(other.cut_off), outer_cut_off(other.outer_cut_off), cast_shadows(other.cast_shadows),
			frustum(other.frustum), near_plane(other.near_plane), far_plane(other.far_plane),
			m_light_type(other.m_light_type), m_shadow_map_frame_buffer(other.m_shadow_map_frame_buffer)
		{
			// set component type
			m_type = "Light";
		}

		Light::Light(Light::Type type, const glm::vec4& color, float intensity, float range, bool cast_shadows, float near_plane, float far_plane) : 
			color(color), intensity(intensity), range(range), cut_off(glm::cos(glm::radians(12.5f))), outer_cut_off(glm::cos(glm::radians(17.5f))), cast_shadows(cast_shadows),
			frustum(-10.0f, 10.0f, -10.0f, 10.0f), near_plane(near_plane), far_plane(far_plane), m_light_type(type), m_shadow_map_frame_buffer(nullptr)
		{
			// set component type
			m_type = "Light";
		}

		Light::~Light()
		{
			// remove light from render
			removeFromVideoLights();
		}

		void Light::operator=(const Light& other)
		{
			color = other.color;
			intensity = other.intensity;
			range = other.range;
			cut_off = other.cut_off;
			outer_cut_off = other.outer_cut_off;
			cast_shadows = other.cast_shadows;
			frustum = other.frustum;
			near_plane = other.near_plane;
			far_plane = other.far_plane;
			m_light_type = other.m_light_type;
			m_shadow_map_frame_buffer = other.m_shadow_map_frame_buffer;
		}

		Light* Light::copy()
		{
			return new Light(*this);
		}

		void Light::sendMessage(const char* message)
		{
			// return if not enable
			if (!enable) return;

			// events
			if (std::strcmp(message, "Start") == 0)
				start();
			else if (std::strcmp(message, "Destroy") == 0)
				destroy();
		}

		void Light::release()
		{
			// free shadow map frame buffer
			if (m_shadow_map_frame_buffer)
			{
				m_shadow_map_frame_buffer->release();

				m_shadow_map_frame_buffer = nullptr;
			}

			// free object
			delete this;
		}

		void Light::start()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// make shadow map frame buffer
			m_shadow_map_frame_buffer = new Assets::Framebuffer();

			if (!m_shadow_map_frame_buffer->build(1024, 1024, (m_light_type != Type::Point) ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP,
				(m_light_type != Type::Point) ? GL_CLAMP_TO_BORDER : GL_CLAMP_TO_EDGE, GL_NEAREST, glm::vec4(1.0f), 
				Assets::Framebuffer::Attachment::Depth))
			{
				app.logger.error("Failed to create light shadow map frame buffer!");
				app.logger.info(m_name.c_str());

				// release frame buffer
				m_shadow_map_frame_buffer->release();

				m_shadow_map_frame_buffer = nullptr;
			}

			// add light to be rendered
			addToVideoLights();
		}

		void Light::destroy()
		{
			// remove light from render
			removeFromVideoLights();
		}

		glm::mat4 Light::getViewMatrix() const
		{
			if (m_light_type == Type::Directional)
				return glm::lookAt(-entity->transform->getWorldRotation(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			else
				return glm::lookAt(entity->transform->getWorldPosition(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		glm::mat4 Light::getProjectionMatrix() const
		{
			if (m_light_type == Type::Directional)
				return glm::ortho(frustum.x, frustum.y, frustum.z, frustum.w, near_plane, far_plane);
			else
				return glm::perspective(glm::radians(90.0f), (float)m_shadow_map_frame_buffer->getWidth() / (float)m_shadow_map_frame_buffer->getHeight(), near_plane, far_plane);
		}

		std::vector<glm::mat4> Light::getOmnidirectionMatrices(const glm::mat4& projection) const
		{
			// light position
			glm::vec3 position = entity->transform->getWorldPosition();

			// matrices list in order of right, left, top, bottom, back, front
			return std::vector<glm::mat4> {
				projection * glm::lookAt(position, position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
				projection * glm::lookAt(position, position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
				projection * glm::lookAt(position, position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
				projection * glm::lookAt(position, position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
				projection * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
				projection * glm::lookAt(position, position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
			};
		}

		void Light::addToVideoLights()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// add to lights list
			app.video.lights.push_back(this);
		}

		void Light::removeFromVideoLights()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// light iterator
			auto iterator = std::find(app.video.lights.begin(), app.video.lights.end(), this);

			// remove from lights list
			if (iterator != app.video.lights.end())
				app.video.lights.erase(iterator);
		}

		Light::Type Light::getLightType() const
		{
			return m_light_type;
		}

		Assets::Framebuffer* Light::getShadowMapFramebuffer() const
		{
			return m_shadow_map_frame_buffer;
		}
	}
}