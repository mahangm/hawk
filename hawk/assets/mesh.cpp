#include "mesh.hpp"
#include <glad/glad.h>
#include "../helpers/math.hpp"

namespace hawk
{
	namespace Assets
	{
		Mesh::Mesh() : mode(GL_TRIANGLES), m_vao(0), m_vbo(0), m_ibo(0)
		{
			// set asset type
			m_type = "Mesh";
		}

		Mesh::Mesh(const Mesh& other) : Asset(other), vertices(other.vertices), indices(other.indices), mode(other.mode),
			m_vao(other.m_vao), m_vbo(other.m_vbo), m_ibo(other.m_ibo) 
		{
			// set asset type
			m_type = "Mesh";
		}

		Mesh::Mesh(const std::vector<Vertex>& vertices, unsigned int mode) : vertices(vertices), mode(mode)
		{
			// set asset type
			m_type = "Mesh";
		}

		Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, unsigned int mode) : vertices(vertices), 
			indices(indices), mode(mode)
		{
			// set asset type
			m_type = "Mesh";
		}

		Mesh::Mesh(Mesh::Primitive primitive, float width, float height, float length)
		{
			// set asset type
			m_type = "Mesh";

			// make primitive
			if (primitive == Primitive::Quad)
			{
				// set vertices for a quad
				vertices = std::vector<Vertex>{
					Vertex { glm::vec3(-width, height, 0.0f), glm::vec2(0.0f, height), glm::vec3(0.0f, 0.0f, 1.0f) },
					Vertex { glm::vec3(-width, -height, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
					Vertex { glm::vec3(width, -height, 0.0f), glm::vec2(width, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
					Vertex { glm::vec3(width, height, 0.0f), glm::vec2(width, height), glm::vec3(0.0f, 0.0f, 1.0f) }
				};

				// set indices
				indices = std::vector<unsigned int>{
					// front
					0, 1, 2,
					2, 3, 0
				};

				// set draw mode
				mode = GL_TRIANGLES;
			}
			else if (primitive == Primitive::Cube)
			{
				// set vertices for a cube
				vertices = std::vector<Vertex>{
					// front
					Vertex { glm::vec3(-width, height, length), glm::vec2(0.0f, height), glm::vec3(0.0f, 0.0f, 1.0f) },
					Vertex { glm::vec3(-width, -height, length), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
					Vertex { glm::vec3(width, -height, length), glm::vec2(width, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f) },
					Vertex { glm::vec3(width, height, length), glm::vec2(width, height), glm::vec3(0.0f, 0.0f, 1.0f) },
					// left
					Vertex { glm::vec3(-width, height, -length), glm::vec2(0.0f, height), glm::vec3(-1.0f, 0.0f, 0.0f) },
					Vertex { glm::vec3(-width, -height, -length), glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f) },
					Vertex { glm::vec3(-width, -height, length), glm::vec2(length, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f) },
					Vertex { glm::vec3(-width, height, length), glm::vec2(length, height), glm::vec3(-1.0f, 0.0f, 0.0f) },
					// back
					Vertex { glm::vec3(width, height, -length), glm::vec2(0.0f, height), glm::vec3(0.0f, 0.0f, -1.0f) },
					Vertex { glm::vec3(width, -height, -length), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
					Vertex { glm::vec3(-width, -height, -length), glm::vec2(width, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
					Vertex { glm::vec3(-width, height, -length), glm::vec2(width, height), glm::vec3(0.0f, 0.0f, -1.0f) },
					// right
					Vertex { glm::vec3(width, height, length), glm::vec2(0.0f, height), glm::vec3(1.0f, 0.0f, 0.0f) },
					Vertex { glm::vec3(width, -height, length), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
					Vertex { glm::vec3(width, -height, -length), glm::vec2(length, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f) },
					Vertex { glm::vec3(width, height, -length), glm::vec2(length, height), glm::vec3(1.0f, 0.0f, 0.0f) },
					// top
					Vertex { glm::vec3(-width, height, -length), glm::vec2(0.0f, length), glm::vec3(0.0f, 1.0f, 0.0f) },
					Vertex { glm::vec3(-width, height, length), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) },
					Vertex { glm::vec3(width, height, length), glm::vec2(width, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) },
					Vertex { glm::vec3(width, height, -length), glm::vec2(width, length), glm::vec3(0.0f, 1.0f, 0.0f) },
					// bottom
					Vertex { glm::vec3(-width, -height, length), glm::vec2(0.0f, length), glm::vec3(0.0f, -1.0f, 0.0f) },
					Vertex { glm::vec3(-width, -height, -length), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
					Vertex { glm::vec3(width, -height, -length), glm::vec2(width, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f) },
					Vertex { glm::vec3(width, -height, length), glm::vec2(width, length), glm::vec3(0.0f, -1.0f, 0.0f) }
				};

				// set indices
				indices = std::vector<unsigned int>{
					// front
					0, 1, 2,
					2, 3, 0,
					// left
					4, 5, 6,
					6, 7, 4,
					// back
					8, 9, 10,
					10, 11, 8,
					// right
					12, 13, 14,
					14, 15, 12,
					// top
					16, 17, 18,
					18, 19, 16,
					// bottom
					20, 21, 22,
					22, 23, 20
				};

				// set draw mode
				mode = GL_TRIANGLES;
			}
			else if (primitive == Primitive::Sphere)
			{
				// sphere segments
				int x_segments = 32, y_segments = 32;

				// set vertices for a sphere
				for (int y = 0; y <= y_segments; ++y)
				{
					for (int x = 0; x <= x_segments; ++x)
					{
						// sphere theta
						float x_segment = (float)x / (float)x_segments;

						// sphere phi
						float y_segment = (float)y / (float)y_segments;

						// vertex position
						glm::vec3 position((std::cos(x_segment * 2.0f * Helpers::Math::PI) * std::sin(y_segment * Helpers::Math::PI)) * width,
							std::cos(y_segment * Helpers::Math::PI) * height,
							(std::sin(x_segment * 2.0f * Helpers::Math::PI) * std::sin(y_segment * Helpers::Math::PI)) * length);

						vertices.push_back({ position, glm::vec2(x_segment, y_segment), position });
					}
				}

				// set indices
				for (int y = 0; y < y_segments; ++y)
				{
					for (int x = 0; x < x_segments; ++x)
					{
						// first triangle
						indices.push_back((y + 1) * (x_segments + 1) + x);
						indices.push_back(y * (x_segments + 1) + x);
						indices.push_back(y * (x_segments + 1) + x + 1);

						// second triangle
						indices.push_back((y + 1) * (x_segments + 1) + x);
						indices.push_back(y * (x_segments + 1) + x + 1);
						indices.push_back((y + 1) * (x_segments + 1) + x + 1);
					}
				}

				// set draw mode
				mode = GL_TRIANGLES;
			}

			// calculate shape tangent vectors
			calculateTangents(vertices, indices);
		}

		Mesh::~Mesh() {}

		void Mesh::operator=(const Mesh& other)
		{
			vertices = other.vertices;
			indices = other.indices;
			mode = other.mode;
			m_vao = other.m_vao;
			m_vbo = other.m_vbo;
			m_ibo = other.m_ibo;
		}

		bool Mesh::build()
		{
			// make vertex array
			glGenVertexArrays(1, &m_vao);

			// make vertex buffer
			glGenBuffers(1, &m_vbo);

			// make index buffer if we have vertex indices
			if (indices.size() > 0) glGenBuffers(1, &m_ibo);

			// bind vertex array to hold changes
			glBindVertexArray(m_vao);

			// bind and fill vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof Vertex, &vertices[0], GL_STATIC_DRAW);

			// bind and fill index buffer if any
			if (m_ibo)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
			}

			// set vertex position
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof Vertex, (void*)0);

			// set vertex texture
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof Vertex, (void*)offsetof(Vertex, uv));

			// set vertex normal
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof Vertex, (void*)offsetof(Vertex, normal));

			// set vertex normal tangent
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof Vertex, (void*)offsetof(Vertex, tangent));

			// set vertex normal bitangent
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof Vertex, (void*)offsetof(Vertex, bitangent));

			// unbind vertex array
			glBindVertexArray(0);

			// unbind vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			// unbind index buffer if any
			if (m_ibo) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			return true;
		}

		void Mesh::release()
		{
			// free vertex array
			if (m_vao)
			{
				glDeleteVertexArrays(1, &m_vao);

				m_vao = 0;
			}

			// free vertex buffer
			if (m_vbo)
			{
				glDeleteBuffers(1, &m_vbo);

				m_vbo = 0;
			}

			// free index buffer
			if (m_ibo)
			{
				glDeleteBuffers(1, &m_ibo);

				m_ibo = 0;
			}

			// free object
			delete this;
		}

		unsigned int Mesh::getVao() const
		{
			return m_vao;
		}

		void Mesh::calculateTangents(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
		{
			// triangle edge vector
			glm::vec3 edge1, edge2 = glm::vec3(0.0f);

			// texture delta uv vector
			glm::vec2 delta_uv1, delta_uv2 = glm::vec2(0.0f);

			for (unsigned int i = 0; i < indices.size(); i += 3)
			{
				// triangle vertex
				Vertex& v0 = vertices[indices[i]];
				Vertex& v1 = vertices[indices[i + 1]];
				Vertex& v2 = vertices[indices[i + 2]];

				// calculate edges
				edge1 = v0.position - v1.position;
				edge2 = v2.position - v1.position;

				// calculate delta uvs
				delta_uv1 = v0.uv - v1.uv;
				delta_uv2 = v2.uv - v1.uv;

				// calculate fraction
				float fraction = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv2.x * delta_uv1.y);

				// calculate tangent
				glm::vec3 tangent = glm::normalize(glm::vec3(
					fraction * (delta_uv2.y * edge1.x - delta_uv1.y * edge2.x),
					fraction * (delta_uv2.y * edge1.y - delta_uv1.y * edge2.y),
					fraction * (delta_uv2.y * edge1.z - delta_uv1.y * edge2.z)));

				// calculate bitangent
				glm::vec3 bitangent = glm::normalize(glm::vec3(
					fraction * (-delta_uv2.x * edge1.x + delta_uv1.x * edge2.x),
					fraction * (-delta_uv2.x * edge1.y + delta_uv1.x * edge2.y),
					fraction * (-delta_uv2.x * edge1.z + delta_uv1.x * edge2.z)));

				// set vectors
				v0.tangent = v1.tangent = tangent;
				v0.bitangent = v1.bitangent = bitangent;
			}
		}
	}
}