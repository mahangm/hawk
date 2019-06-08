#ifndef HAWK_ASSETS_MESH_H
#define HAWK_ASSETS_MESH_H

// stl
#include <vector>

// glad
#include <glad/glad.h>

// glm
#include <glm/glm.hpp>

// local
#include "asset.hpp"

namespace hawk
{
	namespace Assets
	{
		/**
		 * Mesh asset.
		 */
		class Mesh : public Asset
		{
		public:
			/**
			 * Vertex representation.
			 */
			struct Vertex
			{
				/**
				 * Vertex position.
				 */
				glm::vec3 position;

				/**
				 * Texture coordinate.
				 */
				glm::vec2 uv;

				/**
				 * Normal direction.
				 */
				glm::vec3 normal;

				/**
				 * Normal tangent.
				 */
				glm::vec3 tangent;

				/**
				 * Normal bitangent.
				 */
				glm::vec3 bitangent;
			};

			/**
			 * Primitive enum.
			 */
			enum class Primitive
			{
				Quad = 1,
				Cube,
				Sphere
			};

			/**
			 * Constructor.
			 */
			Mesh();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Mesh(const Mesh& other);

			/**
			 * Constructor.
			 * @param vertices List of mesh vertices.
			 * @param mode Drawing mode.
			 */
			Mesh(const std::vector<Vertex>& vertices, unsigned int mode = GL_TRIANGLES);

			/**
			 * Constructor.
			 * @param vertices List of mesh vertices.
			 * @param indices List of mesh elements.
			 * @param mode Drawing mode.
			 */
			Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, unsigned int mode = GL_TRIANGLES);

			/**
			 * Constructor.
			 * @param primitive Primitive shape type.
			 * @param width Mesh width in normalized device coordinates.
			 * @param height Mesh height in normalized device coordinates.
			 * @param length Mesh length in normalized device coordinates.
			 */
			Mesh(Primitive primitive, float width = 1.0f, float height = 1.0f, float length = 1.0f);

			/**
			 * Destructor.
			 */
			~Mesh();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Mesh& other);

			/**
			 * Generate buffers and set their data.
			 * @return True or False.
			 */
			bool build();

			/**
			 * Release resources.
			 */
			void release();

			/**
			 * Vertex array object.
			 */
			unsigned int getVao() const;

			/**
			 * List of mesh vertices.
			 */
			std::vector<Vertex> vertices;

			/**
			 * List of mesh elements.
			 */
			std::vector<unsigned int> indices;

			/**
			 * Drawing mode.
			 */
			unsigned int mode;

		private:
			/**
			 * Calculate triangle tangent vectors.
			 * @param vertices Triangle vertices.
			 * @param indices Shape indices.
			 */
			void calculateTangents(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

			/**
			 * Vertex array object.
			 */
			unsigned int m_vao;

			/**
			 * Vertex buffer object.
			 */
			unsigned int m_vbo;

			/**
			 * Index buffer object.
			 */
			unsigned int m_ibo;
		};
	}
}
#endif