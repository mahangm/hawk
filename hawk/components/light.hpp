#ifndef HAWK_COMPONENTS_LIGHT_H
#define HAWK_COMPONENTS_LIGHT_H

// stl
#include <vector>

// glm
#include <glm/glm.hpp>

// local
#include "component.hpp"
#include "../assets/framebuffer.hpp"

namespace hawk
{
	namespace Components
	{
		/**
		 * Light component.
		 */
		class Light : public Component
		{
		public:
			/**
			 * Type enum.
			 */
			enum class Type : int
			{
				Directional = 1,
				Point,
				Spot
			};

			/**
			 * Constructor.
			 */
			Light();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Light(const Light& other);

			/**
			 * Constructor.
			 * @param type Light type.
			 * @param color Light color.
			 * @param intensity Light intensity.
			 * @param range Attenuation range.
			 * @param cast_shadows Is light casting shadows?
			 * @param near_plane Shadow frustum near plane.
			 * @param far_plane Shadow frustum far plane.
			 */
			Light(Type type, const glm::vec4& color = glm::vec4(1.0f), float intensity = 1.0f, float range = 5.0f, bool cast_shadows = false, float near_plane = 1.0f, float far_plane = 10.0f);

			/**
			 * Destructor.
			 */
			~Light();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Light& other);

			/**
			 * Make a copy from this object.
			 */
			Light* copy();

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
			 * Start event.
			 */
			void start();

			/**
			 * Destroy event.
			 */
			void destroy();

			/**
			 * View matrix.
			 */
			glm::mat4 getViewMatrix() const;

			/**
			 * Projection matrix.
			 */
			glm::mat4 getProjectionMatrix() const;

			/**
			 * Omnidirectional matrices.
			 * @param projection Light projection matrix.
			 */
			std::vector<glm::mat4> getOmnidirectionMatrices(const glm::mat4& projection) const;

			/**
			 * Get light type.
			 */
			Type getLightType() const;

			/**
			 * Shadow map frame buffer.
			 */
			Assets::Framebuffer* getShadowMapFramebuffer() const;

			/**
			 * Light color.
			 */
			glm::vec4 color;

			/**
			 * Light intensity. Multiplied into light color.
			 */
			float intensity;

			/**
			 * Attenuation range.
			 */
			float range;

			/**
			 * Spot inner cone angle in cosine radians.
			 */
			float cut_off;

			/**
			 * Spot outer cone angle in cosine radians.
			 */
			float outer_cut_off;

			/**
			 * Is this light casting shadows on objects?
			 */
			bool cast_shadows;

			/**
			 * Orthographic shadow frustum box size. The order is left, right, bottom, top.
			 */
			glm::vec4 frustum;

			/**
			 * Shadow frustum near plane.
			 */
			float near_plane;
			
			/**
			 * Shadow frustum far plane.
			 */
			float far_plane;

		private:
			/**
			 * Add light to render.
			 */
			void addToVideoLights();

			/**
			 * Remove light from render.
			 */
			void removeFromVideoLights();

			/**
			 * Light type.
			 */
			Type m_light_type;

			/**
			 * Shadow map frame buffer.
			 */
			Assets::Framebuffer* m_shadow_map_frame_buffer;
		};
	}
}
#endif