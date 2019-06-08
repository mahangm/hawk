#ifndef HAWK_COMPONENTS_RENDERER_H
#define HAWK_COMPONENTS_RENDERER_H

// glm
#include <glm/glm.hpp>

// local
#include "component.hpp"
#include "transform.hpp"
#include "light.hpp"
#include "../assets/material.hpp"
#include "../assets/mesh.hpp"

namespace hawk
{
	namespace Components
	{
		/**
		 * Renderer base component.
		 */
		class Renderer : public Component
		{
		public:
			/**
			 * Shadow contribution enum.
			 */
			enum class Shadows
			{
				Off,
				On = 1,
				OnlyShadows
			};

			/**
			 * Constructor.
			 */
			Renderer();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Renderer(const Renderer& other);

			/**
			 * Destructor.
			 */
			virtual ~Renderer();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Renderer& other);

			/**
			 * Make a copy from this object.
			 */
			virtual Renderer* copy();

			/**
			 * Send message to trigger events.
			 * @param message Message context.
			 */
			virtual void sendMessage(const char* message);

			/**
			 * Release resources.
			 */
			virtual void release();

			/**
			 * Render event.
			 */
			virtual void render();

			/**
			 * Render with manual material and mesh.
			 * @param material Material to draw.
			 * @param mesh Mesh to draw.
			 */
			virtual void render(Assets::Material* material, Assets::Mesh* mesh);

			/**
			 * Render for shadow map.
			 * @param material Shadow mapping material.
			 * @param light Render light.
			 */
			virtual void render(Assets::Material* material, const Light* light);

			/**
			 * Destroy event.
			 */
			virtual void destroy();

			/**
			 * Render callback type.
			 */
			typedef void(RenderCallback)(Renderer*);

			/**
			 * Render event.
			 */
			RenderCallback* onRender;

			/**
			 * Shadow contribution.
			 */
			Shadows shadows;

			/**
			 * Is object casting shadows?
			 */
			bool cast_shadows;

			/**
			 * List of materials.
			 */
			std::vector<Assets::Material*> materials;

			/**
			 * Render mesh.
			 */
			Assets::Mesh* mesh;

			/**
			 * Polygon fill mode.
			 */
			unsigned int mode;

		protected:
			/**
			 * Add renderer to render.
			 */
			void addToVideoRenderers();

			/**
			 * Remove renderer from render.
			 */
			void removeFromVideoRenderers();
		};
	}
}
#endif