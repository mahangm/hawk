#ifndef HAWK_COMPONENTS_MESH_RENDERER_H
#define HAWK_COMPONENTS_MESH_RENDERER_H

// glm
#include <glm/glm.hpp>

// local
#include "renderer.hpp"
#include "transform.hpp"
#include "light.hpp"
#include "../assets/material.hpp"
#include "../assets/mesh.hpp"

namespace hawk
{
	namespace Components
	{
		/**
		 * Mesh renderer component.
		 */
		class MeshRenderer : public Renderer
		{
		public:
			/**
			 * Constructor.
			 */
			MeshRenderer();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			MeshRenderer(const MeshRenderer& other);

			/**
			 * Constructor.
			 * @param material Render material.
			 * @param mesh Render mesh.
			 * @param mode Polygon fill mode.
			 */
			MeshRenderer(Assets::Material* material, Assets::Mesh* mesh, Shadows shadows = Shadows::On, unsigned int mode = GL_FILL);

			/**
			 * Destructor.
			 */
			virtual ~MeshRenderer();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const MeshRenderer& other);

			/**
			 * Make a copy from this object.
			 */
			MeshRenderer* copy();

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
			 * Update event.
			 */
			void update();

			/**
			 * Render event.
			 */
			void render();

			/**
			 * Render with provided material and mesh.
			 * @param material Material to draw.
			 * @param mesh Mesh to draw.
			 */
			void render(Assets::Material* material, Assets::Mesh* mesh);

			/**
			 * Render for shadow map.
			 * @param material Shadow mapping material.
			 * @param light Render light.
			 */
			void render(Assets::Material* material, const Light* light);

			/**
			 * Destroy event.
			 */
			void destroy();

			/**
			 * Render callback type.
			 */
			typedef void(RenderCallback)(MeshRenderer*);

			/**
			 * Render event.
			 */
			RenderCallback* onRender;

			/**
			 * Instancing transforms.
			 */
			std::vector<Transform*> instances;

		private:
			/**
			 * Get model matrices from transform.
			 * @param model Model matrix.
			 * @param normal Normal matrix.
			 */
			void getModelMatrices(glm::mat4& model, glm::mat4& normal);

			/**
			 * Instancing matrices buffer.
			 */
			unsigned int m_instance_buffer;
		};
	}
}
#endif