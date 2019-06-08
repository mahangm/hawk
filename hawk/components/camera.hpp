#ifndef HAWK_COMPONENTS_CAMERA_H
#define HAWK_COMPONENTS_CAMERA_H

// glm
#include <glm/glm.hpp>

// local
#include "component.hpp"
#include "../assets/material.hpp"
#include "../assets/framebuffer.hpp"

namespace hawk
{
	namespace Components
	{
		/**
		 * Camera component.
		 */
		class Camera : public Component
		{
		public:
			/**
			 * Projection enum.
			 */
			enum class Projection
			{
				Orthographic = 1,
				Perspective
			};

			/**
			 * Constructor.
			 */
			Camera();

			/**
			 * Constructor.
			 * @param projection Projection type.
			 * @param background Background color.
			 * @param viewport Camera viewport.
			 */
			Camera(Camera::Projection projection, const glm::vec4& background, const glm::ivec4& viewport);

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Camera(const Camera& other);

			/**
			 * Destructor.
			 */
			~Camera();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Camera& other);

			/**
			 * Make a copy from this object.
			 */
			Camera* copy();

			/**
			 * Send message to trigger events.
			 * @param message Message context.
			 */
			void sendMessage(const char* message);

			/**
			 * Release resources.
			 */
			void release();

			/**
			 * Start.
			 */
			void start();

			/**
			 * Update.
			 */
			void update();

			/**
			 * Pre-render.
			 */
			void preRender();

			/**
			 * Post-render.
			 */
			void postRender();

			/**
			 * Destroy event.
			 */
			void destroy();

			/**
			 * Fill camera uniform buffer.
			 * @param ubo Uniform buffer object.
			 */
			void fillCameraBuffer(unsigned int ubo);

			/**
			 * View matrix.
			 */
			glm::mat4 getViewMatrix() const;

			/**
			 * Projection matrix.
			 */
			glm::mat4 getProjectionMatrix() const;

			/**
			 * Default frame buffer.
			 */
			Assets::Framebuffer* getDefaultFramebuffer() const;

			/**
			 * Deferred frame buffer.
			 */
			Assets::Framebuffer* getDeferredFramebuffer() const;

			/**
			 * Screen frame buffer.
			 */
			Assets::Framebuffer* getScreenFramebuffer() const;

			/**
			 * Final frame buffer.
			 */
			Assets::Framebuffer* getFinalFramebuffer() const;

			/**
			 * Pre-render callback type.
			 */
			typedef void(PreRenderCallback)(Camera*);

			/**
			 * Post-render callback type.
			 */
			typedef void(PostRenderCallback)(Camera*);

			/**
			 * Pre-render event.
			 */
			PreRenderCallback* onPreRender;

			/**
			 * Post-render event.
			 */
			PostRenderCallback* onPostRender;

			/**
			 * Projection type.
			 */
			Projection projection;

			/**
			 * Background color.
			 */
			glm::vec4 background;

			/**
			 * Background skybox.
			 */
			Assets::Material* skybox;

			/**
			 * Irradiance cube map for indirect diffuse lighting.
			 */
			Assets::Texture* irradiance;

			/**
			 * Radiance cube map for indirect specular lighting.
			 */
			Assets::Texture* radiance;

			/**
			 * Render viewport.
			 */
			glm::ivec4 viewport;

			/**
			 * Post-process materials.
			 */
			std::vector<Assets::Material*> materials;

			/**
			 * Field of view.
			 */
			float fov;

			/**
			 * Near frustum plane.
			 */
			float near_plane;

			/**
			 * Far frustum plane.
			 */
			float far_plane;

			/**
			 * Camera front vector.
			 */
			glm::vec3 front;

			/**
			 * Camera up vector.
			 */
			glm::vec3 up;

			/**
			 * Camera right vector.
			 */
			glm::vec3 right;

		private:
			/**
			 * Calculate camera vectors.
			 */
			void calculate();

			/**
			 * Add camera to render.
			 */
			void addToVideoCameras();

			/**
			 * Remove camera from render.
			 */
			void removeFromVideoCameras();

			/**
			 * Default frame buffer for forward and intermediate deferred rendering.
			 */
			Assets::Framebuffer* m_default_framebuffer;

			/**
			 * Deferred frame buffer which is a G-Buffer.
			 */
			Assets::Framebuffer* m_deferred_framebuffer;

			/**
			 * Screen frame buffer to draw raw screen quad and apply effects in post-process.
			 */
			Assets::Framebuffer* m_screen_framebuffer;

			/**
			 * Final frame buffer to apply gamma correction and hdr in post-process.
			 */
			Assets::Framebuffer* m_final_framebuffer;
		};
	}
}
#endif