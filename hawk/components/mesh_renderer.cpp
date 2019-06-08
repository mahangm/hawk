#include "../system/core.hpp"
#include "mesh_renderer.hpp"
#include "../objects/entity.hpp"
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

namespace hawk
{
	namespace Components
	{
		MeshRenderer::MeshRenderer() : onRender(nullptr), m_instance_buffer(0)
		{
			// set component type
			m_type = "MeshRenderer";
		}

		MeshRenderer::MeshRenderer(const MeshRenderer& other) : Renderer(other), onRender(other.onRender), instances(other.instances), m_instance_buffer(other.m_instance_buffer)
		{
			// set component type
			m_type = "MeshRenderer";
		}

		MeshRenderer::MeshRenderer(hawk::Assets::Material* material, hawk::Assets::Mesh* mesh, Shadows shadows, unsigned int mode) : onRender(nullptr), m_instance_buffer(0)
		{
			// set component type
			m_type = "MeshRenderer";

			// set properties
			this->shadows = shadows;
			this->mesh = mesh;
			this->mode = mode;

			// push to the list
			materials.push_back(material);
		}

		MeshRenderer::~MeshRenderer()
		{
			// remove from render
			removeFromVideoRenderers();
		}

		MeshRenderer* MeshRenderer::copy()
		{
			return new MeshRenderer(*this);
		}

		void MeshRenderer::operator=(const MeshRenderer& other)
		{
			onRender = other.onRender;
			instances = other.instances;
			m_instance_buffer = other.m_instance_buffer;
		}

		void MeshRenderer::sendMessage(const char* message)
		{
			// return if not enable
			if (!enable) return;

			// events
			if (std::strcmp(message, "Start") == 0)
				start();
			else if (std::strcmp(message, "Update") == 0)
				update();
			else if (std::strcmp(message, "Render") == 0)
				render();
			else if (std::strcmp(message, "Destroy") == 0)
				destroy();
		}

		void MeshRenderer::release()
		{
			// free models buffer
			if (m_instance_buffer)
			{
				glDeleteBuffers(1, &m_instance_buffer);

				m_instance_buffer = 0;
			}

			// free object
			delete this;
		}

		void MeshRenderer::start()
		{
			// if instancing is required
			if (instances.size() > 0)
			{
				// list of instances model matrices
				std::vector<glm::mat4> models;

				// world position vector
				const glm::vec3 world_position = entity->transform->getWorldPosition();

				// world position vector
				const glm::vec3 world_rotation = entity->transform->getWorldRotation();

				// world position vector
				const glm::vec3 world_scale = entity->transform->getWorldScale();

				// set matrices
				for (int i = 0; i < instances.size(); ++i)
				{
					// calculate instance vectors
					instances[i]->calculate(world_position, world_rotation, world_scale);

					// push to the list
					models.push_back(instances[i]->getModelMatrix());
				}

				// make instances buffer
				glGenBuffers(1, &m_instance_buffer);

				// bind mesh vertex array
				glBindVertexArray(mesh->getVao());

				// bind and fill instances buffer
				glBindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
				glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof glm::mat4, &models[0], GL_DYNAMIC_DRAW);

				// vec4 size
				std::size_t vec4_size = sizeof glm::vec4;

				// set model matrix first row and change once per instance
				glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof glm::mat4, (void*)0);
				glEnableVertexAttribArray(5);
				glVertexAttribDivisor(5, 1);

				// set model matrix second row and change once per instance
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof glm::mat4, (void*)(1 * vec4_size));
				glEnableVertexAttribArray(6);
				glVertexAttribDivisor(6, 1);

				// set model matrix third row and change once per instance
				glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof glm::mat4, (void*)(2 * vec4_size));
				glEnableVertexAttribArray(7);
				glVertexAttribDivisor(7, 1);

				// set model matrix fourth row and change once per instance
				glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof glm::mat4, (void*)(3 * vec4_size));
				glEnableVertexAttribArray(8);
				glVertexAttribDivisor(8, 1);

				// unbind vertex array
				glBindVertexArray(0);

				// unbind instance buffer
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}

			// add mesh renderer to be rendered
			addToVideoRenderers();
		}

		void MeshRenderer::update()
		{
			// if instancing is required
			if (instances.size() > 0)
			{
				// list of instances model matrices
				std::vector<glm::mat4> models;

				// world position vector
				const glm::vec3 world_position = entity->transform->getWorldPosition();

				// world position vector
				const glm::vec3 world_rotation = entity->transform->getWorldRotation();

				// world position vector
				const glm::vec3 world_scale = entity->transform->getWorldScale();

				// set matrices
				for (int i = 0; i < instances.size(); ++i)
				{
					// calculate instance vectors
					instances[i]->calculate(world_position, world_rotation, world_scale);

					// push to the list
					models.push_back(instances[i]->getModelMatrix());
				}

				// fill instances buffer
				glBindBuffer(GL_ARRAY_BUFFER, m_instance_buffer);
				glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof glm::mat4, &models[0], GL_DYNAMIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
		}

		void MeshRenderer::render()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// model matrix
			glm::mat4 model = glm::mat4(1.0f);

			// normal matrix
			glm::mat4 normal = glm::mat4(1.0f);

			// get model matrices
			getModelMatrices(model, normal);

			// set fill mode
			glPolygonMode(GL_FRONT_AND_BACK, mode);

			// render
			for (auto material : materials)
			{
				// set model and normal matrices
				material->setFloatMat4("transform.model", model);
				material->setFloatMat4("transform.normal", normal);

				// set renderer properties
				material->setInt("renderer.shadows", (int)shadows);

				// use material
				material->use();

				// set material render states
				material->setStates();

				// set shader uniform values
				material->setUniforms();

				// set shader sampler values
				material->setSamplers();

				// draw mesh
				app.video.draw(mesh, (int)instances.size());
			}

			// raise render event
			if (onRender) (*onRender)(this);

			// reset render states
			app.video.reset();
		}

		void MeshRenderer::render(Assets::Material* material, Assets::Mesh* mesh)
		{
			// application
			System::Core& app = System::Core::getInstance();

			// model matrix
			glm::mat4 model = glm::mat4(1.0f);

			// normal matrix
			glm::mat4 normal = glm::mat4(1.0f);

			// get model matrices
			getModelMatrices(model, normal);

			// set model and normal matrices
			material->setFloatMat4("transform.model", model);
			material->setFloatMat4("transform.normal", normal);

			// set renderer properties
			material->setInt("renderer.shadows", (int)shadows);

			// use material
			material->use();

			// set material render states
			material->setStates();

			// set shader uniform values
			material->setUniforms();

			// set shader sampler values
			material->setSamplers();

			// set fill mode
			glPolygonMode(GL_FRONT_AND_BACK, mode);

			// draw mesh
			app.video.draw(mesh, (int)instances.size());

			// reset render states
			app.video.reset();
		}

		void MeshRenderer::render(Assets::Material* material, const Light* light)
		{
			// application
			System::Core& app = System::Core::getInstance();

			// model matrix
			glm::mat4 model = glm::mat4(1.0f);

			// normal matrix
			glm::mat4 normal = glm::mat4(1.0f);

			// get model matrices
			getModelMatrices(model, normal);

			// light projection matrix
			glm::mat4 projection_matrix = light->getProjectionMatrix();

			// set shared properties
			material->setFloatMat4("transform.model", model);

			// set individual properties
			if (light->getLightType() != Light::Type::Point)
			{
				// light space matrix
				glm::mat4 light_matrix = projection_matrix * light->getViewMatrix();

				material->setFloatMat4("light_matrix", light_matrix);
			}
			else
			{
				// set light position
				material->setFloatVec3("light_position", light->entity->transform->getWorldPosition());

				// omnidirectional matrices
				std::vector<glm::mat4> light_matrices = light->getOmnidirectionMatrices(projection_matrix);

				// set ssao sample kernel
				material->setFloatMat4("light_matrices", light_matrices);

				// set far plane
				material->setFloat("light_far_plane", light->far_plane);
			}

			// set shader program
			material->use();

			// set shader uniforms
			material->setUniforms();

			// set fill mode
			glPolygonMode(GL_FRONT_AND_BACK, mode);

			// draw mesh
			app.video.draw(mesh, (int)instances.size());
		}

		void MeshRenderer::destroy()
		{
			// remove mesh renderers from render 
			removeFromVideoRenderers();
		}

		void MeshRenderer::getModelMatrices(glm::mat4& model, glm::mat4& normal)
		{
			// model matrix
			model = entity->transform->getModelMatrix();

			// normal matrix
			normal = glm::transpose(glm::inverse(model));
		}
	}
}