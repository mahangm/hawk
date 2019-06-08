#include "../system/core.hpp"
#include "material.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <sstream>

namespace hawk
{
	namespace Assets
	{
		Material::Material() : color_mask(true), depth_func(GL_LESS), depth_mask(true), stencil_func(GL_ALWAYS), stencil_func_ref(1), stencil_func_mask(0xFF), stencil_mask(0x00), 
			blend_src_factor(GL_ONE), blend_dst_factor(GL_ZERO), blend_equation(GL_FUNC_ADD), blend_color(1.0f), face_cull(true), face_side(GL_BACK), face_orientation(GL_CCW),
			m_shader(nullptr), m_material_type(Type::None)
		{
			// set asset type
			m_type = "Material";
		}

		Material::Material(const Material& other) : Asset(other),
			uniforms(other.uniforms), textures(other.textures), color_mask(other.color_mask), depth_func(other.depth_func), depth_mask(other.depth_mask),
			stencil_func_ref(other.stencil_func_ref), stencil_func_mask(other.stencil_func_mask), stencil_mask(other.stencil_mask),
			blend_src_factor(other.blend_src_factor), blend_dst_factor(other.blend_dst_factor), blend_equation(other.blend_equation), blend_color(other.blend_color), 
			face_cull(other.face_cull), face_side(other.face_side), face_orientation(other.face_orientation), m_shader(other.m_shader), m_material_type(other.m_material_type)
		{
			// set asset type
			m_type = "Material";
		}

		Material::Material(Shader* shader, Material::Type type) : uniforms(shader->getUniforms()), color_mask(true), depth_func(GL_LESS), depth_mask(true), stencil_func(GL_ALWAYS), stencil_func_ref(1), stencil_func_mask(0xFF), stencil_mask(0x00),
			blend_src_factor(GL_ONE), blend_dst_factor(GL_ZERO), blend_equation(GL_FUNC_ADD), blend_color(1.0f), face_cull(true), face_side(GL_BACK), face_orientation(GL_CCW),
			m_shader(shader), m_material_type(type)
		{
			// set asset type
			m_type = "Material";
		}

		Material::~Material() {}

		void Material::operator=(const Material& other)
		{
			uniforms = other.uniforms;
			textures = other.textures;
			color_mask = other.color_mask;
			depth_func = other.depth_func;
			depth_mask = other.depth_mask;
			stencil_func = other.stencil_func;
			stencil_func_ref = other.stencil_func_ref;
			stencil_func_mask = other.stencil_func_mask;
			stencil_mask = other.stencil_mask;
			blend_src_factor = other.blend_src_factor;
			blend_dst_factor = other.blend_dst_factor;
			blend_equation = other.blend_equation;
			blend_color = other.blend_color;
			face_cull = other.face_cull;
			face_side = other.face_side;
			face_orientation = other.face_orientation;
			m_shader = other.m_shader;
			m_material_type = other.m_material_type;
		}

		void Material::release()
		{
			// free uniforms allocated values
			for (auto& uniform : uniforms)
				if (uniform.second.value) delete uniform.second.value;

			// clear lists
			uniforms.clear();
			textures.clear();

			// free object
			delete this;
		}

		void Material::use()
		{
			glUseProgram(m_shader->getProgram());
		}

		Shader* Material::getShader() const
		{
			return m_shader;
		}

		Material::Type Material::getMaterialType() const
		{
			return m_material_type;
		}

		void Material::setStates()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// set color mask
			app.video.setColorMask(color_mask);

			// set depth test
			app.video.setDepthFunction(depth_func);
			app.video.setDepthMask(depth_mask);

			// set stencil test
			app.video.setStencilFunction(stencil_func, stencil_func_ref, stencil_func_mask);
			app.video.setStencilMask(stencil_mask);

			// set blending
			app.video.setBlendFunction(blend_src_factor, blend_dst_factor);
			app.video.setBlendEquation(blend_equation);
			app.video.setBlendColor(blend_color);

			// set face culling
			app.video.setFaceCull(face_cull);
			app.video.setFaceSide(face_side);
			app.video.setFaceOrientation(face_orientation);
		}

		void Material::setUniforms()
		{
			// if we have uniforms
			if (!uniforms.empty())
			{
				// iterate uniforms
				for (auto& item : uniforms)
				{
					// current uniform
					auto& uniform = item.second;

					// continue if uniform location is not found or has no value
					if (uniform.location < 0 || !uniform.value) continue;
					
					// set uniform value
					switch (uniform.type)
					{
					case GL_BOOL:
					case GL_INT:
					case GL_SAMPLER_2D:
					case GL_SAMPLER_CUBE:
						if (uniform.size == 1)
						{
							const int value = *(const int*)uniform.value;

							glProgramUniform1i(m_shader->getProgram(), uniform.location, value);
						}
						else if (uniform.size > 1)
						{
							const std::vector<int> values = *(const std::vector<int>*)uniform.value;

							glProgramUniform1iv(m_shader->getProgram(), uniform.location, (int)values.size(), &values[0]);
						}
						break;
					case GL_BOOL_VEC2:
					case GL_INT_VEC2:
						if (uniform.size == 1)
						{
							const glm::ivec2 value = *(const glm::ivec2*)uniform.value;

							glProgramUniform2i(m_shader->getProgram(), uniform.location, value.x, value.y);
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::ivec2> values = *(const std::vector<glm::ivec2>*)uniform.value;

							glProgramUniform2iv(m_shader->getProgram(), uniform.location, (int)values.size(), glm::value_ptr(values[0]));
						}
						break;
					case GL_BOOL_VEC3:
					case GL_INT_VEC3:
						if (uniform.size == 1)
						{
							const glm::ivec3 value = *(const glm::ivec3*)uniform.value;

							glProgramUniform3i(m_shader->getProgram(), uniform.location, value.x, value.y, value.z);
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::ivec3> values = *(const std::vector<glm::ivec3>*)uniform.value;

							glProgramUniform3iv(m_shader->getProgram(), uniform.location, (int)values.size(), glm::value_ptr(values[0]));
						}
						break;
					case GL_BOOL_VEC4:
					case GL_INT_VEC4:
						if (uniform.size == 1)
						{
							const glm::ivec4 value = *(const glm::ivec4*)uniform.value;

							glProgramUniform4i(m_shader->getProgram(), uniform.location, value.x, value.y, value.z, value.w);
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::ivec4> values = *(const std::vector<glm::ivec4>*)uniform.value;

							glProgramUniform4iv(m_shader->getProgram(), uniform.location, (int)values.size(), glm::value_ptr(values[0]));
						}
						break;
					case GL_UNSIGNED_INT:
						if (uniform.size == 1)
						{
							const unsigned int value = *(const unsigned int*)uniform.value;

							glProgramUniform1ui(m_shader->getProgram(), uniform.location, value);
						}
						else if (uniform.size > 1)
						{
							const std::vector<unsigned int> values = *(const std::vector<unsigned int>*)uniform.value;

							glProgramUniform1uiv(m_shader->getProgram(), uniform.location, (int)values.size(), &values[0]);
						}
						break;
					case GL_UNSIGNED_INT_VEC2:
						if (uniform.size == 1)
						{
							const glm::uvec2 value = *(const glm::uvec2*)uniform.value;

							glProgramUniform2ui(m_shader->getProgram(), uniform.location, value.x, value.y);
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::uvec2> values = *(const std::vector<glm::uvec2>*)uniform.value;

							glProgramUniform2uiv(m_shader->getProgram(), uniform.location, (int)values.size(), glm::value_ptr(values[0]));
						}
						break;
					case GL_UNSIGNED_INT_VEC3:
						if (uniform.size == 1)
						{
							const glm::uvec3 value = *(const glm::uvec3*)uniform.value;

							glProgramUniform3ui(m_shader->getProgram(), uniform.location, value.x, value.y, value.z);
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::uvec3> values = *(const std::vector<glm::uvec3>*)uniform.value;

							glProgramUniform3uiv(m_shader->getProgram(), uniform.location, (int)values.size(), glm::value_ptr(values[0]));
						}
						break;
					case GL_UNSIGNED_INT_VEC4:
						if (uniform.size == 1)
						{
							const glm::uvec4 value = *(const glm::uvec4*)uniform.value;

							glProgramUniform4ui(m_shader->getProgram(), uniform.location, value.x, value.y, value.z, value.w);
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::uvec4> values = *(const std::vector<glm::uvec4>*)uniform.value;

							glProgramUniform4uiv(m_shader->getProgram(), uniform.location, (int)values.size(), glm::value_ptr(values[0]));
						}
						break;
					case GL_FLOAT:
						if (uniform.size == 1)
						{
							const float value = *(const float*)uniform.value;

							glProgramUniform1f(m_shader->getProgram(), uniform.location, value);
						}
						else if (uniform.size > 1)
						{
							const std::vector<float> values = *(const std::vector<float>*)uniform.value;

							glProgramUniform1fv(m_shader->getProgram(), uniform.location, (int)values.size(), &values[0]);
						}
						break;
					case GL_FLOAT_VEC2:
						if (uniform.size == 1)
						{
							const glm::vec2 value = *(const glm::vec2*)uniform.value;

							glProgramUniform2f(m_shader->getProgram(), uniform.location, value.x, value.y);
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::vec2> values = *(const std::vector<glm::vec2>*)uniform.value;

							glProgramUniform2fv(m_shader->getProgram(), uniform.location, (int)values.size(), glm::value_ptr(values[0]));
						}
						break;
					case GL_FLOAT_VEC3:
						if (uniform.size == 1)
						{
							const glm::vec3 value = *(const glm::vec3*)uniform.value;

							glProgramUniform3f(m_shader->getProgram(), uniform.location, value.x, value.y, value.z);
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::vec3> values = *(const std::vector<glm::vec3>*)uniform.value;

							glProgramUniform3fv(m_shader->getProgram(), uniform.location, (int)values.size(), glm::value_ptr(values[0]));
						}
						break;
					case GL_FLOAT_VEC4:
						if (uniform.size == 1)
						{
							const glm::vec4 value = *(const glm::vec4*)uniform.value;

							glProgramUniform4f(m_shader->getProgram(), uniform.location, value.x, value.y, value.z, value.w);
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::vec4> values = *(const std::vector<glm::vec4>*)uniform.value;

							glProgramUniform4fv(m_shader->getProgram(), uniform.location, (int)values.size(), glm::value_ptr(values[0]));
						}
						break;
					case GL_DOUBLE:
						if (uniform.size == 1)
						{
							const double value = *(const double*)uniform.value;

							glProgramUniform1d(m_shader->getProgram(), uniform.location, value);
						}
						else if (uniform.size > 1)
						{
							const std::vector<double> values = *(const std::vector<double>*)uniform.value;

							glProgramUniform1dv(m_shader->getProgram(), uniform.location, (int)values.size(), &values[0]);
						}
						break;
					case GL_DOUBLE_VEC2:
						if (uniform.size == 1)
						{
							const glm::dvec2 value = *(const glm::dvec2*)uniform.value;

							glProgramUniform2d(m_shader->getProgram(), uniform.location, value.x, value.y);
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::dvec2> values = *(const std::vector<glm::dvec2>*)uniform.value;

							glProgramUniform2dv(m_shader->getProgram(), uniform.location, (int)values.size(), glm::value_ptr(values[0]));
						}
						break;
					case GL_DOUBLE_VEC3:
						if (uniform.size == 1)
						{
							const glm::dvec3 value = *(const glm::dvec3*)uniform.value;

							glProgramUniform3d(m_shader->getProgram(), uniform.location, value.x, value.y, value.z);
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::dvec3> values = *(const std::vector<glm::dvec3>*)uniform.value;

							glProgramUniform3dv(m_shader->getProgram(), uniform.location, (int)values.size(), glm::value_ptr(values[0]));
						}
						break;
					case GL_DOUBLE_VEC4:
						if (uniform.size == 1)
						{
							const glm::dvec4 value = *(const glm::dvec4*)uniform.value;

							glProgramUniform4d(m_shader->getProgram(), uniform.location, value.x, value.y, value.z, value.w);
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::dvec4> values = *(const std::vector<glm::dvec4>*)uniform.value;

							glProgramUniform4dv(m_shader->getProgram(), uniform.location, (int)values.size(), glm::value_ptr(values[0]));
						}
						break;
					case GL_FLOAT_MAT2:
						if (uniform.size == 1)
						{
							const glm::fmat2 value = *(const glm::fmat2*)uniform.value;

							glProgramUniformMatrix2fv(m_shader->getProgram(), uniform.location, uniform.size, uniform.transpose, glm::value_ptr(value));
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::fmat2> values = *(const std::vector<glm::fmat2>*)uniform.value;

							glProgramUniformMatrix2fv(m_shader->getProgram(), uniform.location, (int)values.size(), uniform.transpose, glm::value_ptr(values[0]));
						}
						break;
					case GL_FLOAT_MAT3:
						if (uniform.size == 1)
						{
							const glm::fmat3 value = *(const glm::fmat3*)uniform.value;

							glProgramUniformMatrix3fv(m_shader->getProgram(), uniform.location, uniform.size, uniform.transpose, glm::value_ptr(value));
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::fmat3> values = *(const std::vector<glm::fmat3>*)uniform.value;

							glProgramUniformMatrix3fv(m_shader->getProgram(), uniform.location, (int)values.size(), uniform.transpose, glm::value_ptr(values[0]));
						}
						break;
					case GL_FLOAT_MAT4:
						if (uniform.size == 1)
						{
							const glm::fmat4 value = *(const glm::fmat4*)uniform.value;

							glProgramUniformMatrix4fv(m_shader->getProgram(), uniform.location, uniform.size, uniform.transpose, glm::value_ptr(value));
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::fmat4> values = *(const std::vector<glm::fmat4>*)uniform.value;

							glProgramUniformMatrix4fv(m_shader->getProgram(), uniform.location, (int)values.size(), uniform.transpose, glm::value_ptr(values[0]));
						}
						break;
					case GL_DOUBLE_MAT2:
						if (uniform.size == 1)
						{
							const glm::dmat2 value = *(const glm::dmat2*)uniform.value;

							glProgramUniformMatrix2dv(m_shader->getProgram(), uniform.location, uniform.size, uniform.transpose, glm::value_ptr(value));
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::dmat2> values = *(const std::vector<glm::dmat2>*)uniform.value;

							glProgramUniformMatrix2dv(m_shader->getProgram(), uniform.location, (int)values.size(), uniform.transpose, glm::value_ptr(values[0]));
						}
						break;
					case GL_DOUBLE_MAT3:
						if (uniform.size == 1)
						{
							const glm::dmat3 value = *(const glm::dmat3*)uniform.value;

							glProgramUniformMatrix3dv(m_shader->getProgram(), uniform.location, uniform.size, uniform.transpose, glm::value_ptr(value));
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::dmat3> values = *(const std::vector<glm::dmat3>*)uniform.value;

							glProgramUniformMatrix3dv(m_shader->getProgram(), uniform.location, (int)values.size(), uniform.transpose, glm::value_ptr(values[0]));
						}
						break;
					case GL_DOUBLE_MAT4:
						if (uniform.size == 1)
						{
							const glm::dmat4 value = *(const glm::dmat4*)uniform.value;

							glProgramUniformMatrix4dv(m_shader->getProgram(), uniform.location, uniform.size, uniform.transpose, glm::value_ptr(value));
						}
						else if (uniform.size > 1)
						{
							const std::vector<glm::dmat4> values = *(const std::vector<glm::dmat4>*)uniform.value;

							glProgramUniformMatrix4dv(m_shader->getProgram(), uniform.location, (int)values.size(), uniform.transpose, glm::value_ptr(values[0]));
						}
						break;
					}
				}
			}
		}

		void Material::setSamplers()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// texture sampler counter
			int texture_counter = 0;

			// if we have textures
			if (!textures.empty())
			{
				// set textures
				for (auto texture : textures)
				{
					// bind texture
					texture->bind(texture_counter++);

					// break if exceeded textures limit
					if (texture_counter == 16) break;
				}

				// activate default texture
				glActiveTexture(GL_TEXTURE0);
			}

			// if this a forward lit material
			if (app.video.getRendering() == Managers::Video::Rendering::Forward && m_material_type != Type::Unlit)
			{
				// if we have lights
				if (!app.video.lights.empty())
				{
					// shadow map id holder
					unsigned int shadow_map_id = 0;

					// shadow map sampler index
					int dsm_sampler_index = 0, odsm_sampler_index = 0;

					// shadow map texture counter
					int shadow_map_counter = 16;

					// active lights count
					unsigned int lights_count = 0;

					// set maps
					for (unsigned int i = 0; i < app.video.getMaxLights(); ++i)
					{
						// continue if light is out of active lights range
						if (i + 1 > app.video.lights.size()) break;

						// continue if light is disabled
						if (!app.video.lights[i]->enable) continue;

						// if light has shadows
						if (app.video.lights[i]->cast_shadows && shadow_map_id < app.video.getMaxShadows())
						{
							// directional shadow map sample index
							dsm_sampler_index = shadow_map_counter++;

							// omnidirectional shadow map sample index
							odsm_sampler_index = shadow_map_counter++;

							// increase shadow map id
							shadow_map_id++;

							// bind shadow map texture
							switch (app.video.lights[i]->getLightType())
							{
							case Components::Light::Type::Directional:
								app.video.lights[i]->getShadowMapFramebuffer()->bindDepthStencilTexture(dsm_sampler_index);
								break;
							case Components::Light::Type::Point:
								app.video.lights[i]->getShadowMapFramebuffer()->bindDepthStencilTexture(odsm_sampler_index);
								break;
							case Components::Light::Type::Spot:
								app.video.lights[i]->getShadowMapFramebuffer()->bindDepthStencilTexture(dsm_sampler_index);
								break;
							}
						}

						// increase lights count
						lights_count++;
					}

					// update lights count uniform
					m_shader->setUInt("lights_count", lights_count);
				}

				// set environment lighting textures for pbr
				if (app.video.getLighting() == Managers::Video::Lighting::PBR)
				{
					// bind irradiance map
					if (app.video.getRenderCamera()->irradiance) app.video.getRenderCamera()->irradiance->bind(24);

					// bind radiance map
					if (app.video.getRenderCamera()->radiance) app.video.getRenderCamera()->radiance->bind(25);

					// bind brdf texture
					app.assets.getBRDFTexture()->bind(26);
				}
			}
		}

		void Material::setBool(const char* name, const int value)
		{
			if (uniforms.count(name) == 0) return;
			
			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			int* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (int*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new int(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setBool(const char* name, const std::vector<int>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<int>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<int>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<int>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setBoolVec2(const char* name, const glm::ivec2& value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::ivec2* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::ivec2*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::ivec2(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setBoolVec2(const char* name, const std::vector<glm::ivec2>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::ivec2>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::ivec2>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::ivec2>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setBoolVec3(const char* name, const glm::ivec3& value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::ivec3* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::ivec3*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::ivec3(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setBoolVec3(const char* name, const std::vector<glm::ivec3>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::ivec3>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::ivec3>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::ivec3>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setBoolVec4(const char* name, const glm::ivec4& value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::ivec4* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::ivec4*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::ivec4(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setBoolVec4(const char* name, const std::vector<glm::ivec4>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::ivec4>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::ivec4>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::ivec4>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setInt(const char* name, const int value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			int* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (int*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new int(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setInt(const char* name, const std::vector<int>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<int>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<int>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<int>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setIntVec2(const char* name, const glm::ivec2& value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::ivec2* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::ivec2*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::ivec2(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setIntVec2(const char* name, const std::vector<glm::ivec2>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::ivec2>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::ivec2>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::ivec2>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setIntVec3(const char* name, const glm::ivec3& value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::ivec3* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::ivec3*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::ivec3(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setIntVec3(const char* name, const std::vector<glm::ivec3>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::ivec3>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::ivec3>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::ivec3>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setIntVec4(const char* name, const glm::ivec4& value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::ivec4* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::ivec4*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::ivec4(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setIntVec4(const char* name, const std::vector<glm::ivec4>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::ivec4>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::ivec4>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::ivec4>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setUInt(const char* name, const unsigned int value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			unsigned int* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (unsigned int*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new unsigned int(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setUInt(const char* name, const std::vector<unsigned int>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<unsigned int>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<unsigned int>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<unsigned int>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setUIntVec2(const char* name, const glm::uvec2& value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::uvec2* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::uvec2*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::uvec2(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setUIntVec2(const char* name, const std::vector<glm::uvec2>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::uvec2>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::uvec2>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::uvec2>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setUIntVec3(const char* name, const glm::uvec3& value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::uvec3* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::uvec3*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::uvec3(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setUIntVec3(const char* name, const std::vector<glm::uvec3>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::uvec3>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::uvec3>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::uvec3>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setUIntVec4(const char* name, const glm::uvec4& value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::uvec4* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::uvec4*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::uvec4(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setUIntVec4(const char* name, const std::vector<glm::uvec4>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::uvec4>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::uvec4>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::uvec4>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setFloat(const char* name, const float value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			float* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (float*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new float(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setFloat(const char* name, const std::vector<float>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<float>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<float>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<float>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setFloatVec2(const char* name, const glm::vec2& value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::vec2* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::vec2*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::vec2(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setFloatVec2(const char* name, const std::vector<glm::vec2>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::vec2>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::vec2>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::vec2>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setFloatVec3(const char* name, const glm::vec3& value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::vec3* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::vec3*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::vec3(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setFloatVec3(const char* name, const std::vector<glm::vec3>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::vec3>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::vec3>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::vec3>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setFloatVec4(const char* name, const glm::vec4& value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::vec4* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::vec4*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::vec4(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setFloatVec4(const char* name, const std::vector<glm::vec4>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::vec4>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::vec4>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::vec4>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setDouble(const char* name, const double value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			double* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (double*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new double(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setDouble(const char* name, const std::vector<double>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<double>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<double>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<double>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setDoubleVec2(const char* name, const glm::dvec2& value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::dvec2* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::dvec2*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::dvec2(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setDoubleVec2(const char* name, const std::vector<glm::dvec2>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::dvec2>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::dvec2>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::dvec2>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setDoubleVec3(const char* name, const glm::dvec3& value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::dvec3* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::dvec3*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::dvec3(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setDoubleVec3(const char* name, const std::vector<glm::dvec3>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::dvec3>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::dvec3>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::dvec3>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setDoubleVec4(const char* name, const glm::dvec4& value)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::dvec4* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::dvec4*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::dvec4(value);
			}

			uniform.value = uniform_value;
		}

		void Material::setDoubleVec4(const char* name, const std::vector<glm::dvec4>& values)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::dvec4>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::dvec4>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::dvec4>(values);
			}

			uniform.value = uniform_value;
		}

		void Material::setFloatMat2(const char* name, const glm::mat2& value, bool transpose)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::mat2* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::mat2*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::mat2(value);
			}

			uniform.value = uniform_value;
			uniform.transpose = transpose;
		}

		void Material::setFloatMat2(const char* name, const std::vector<glm::mat2>& values, bool transpose)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::mat2>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::mat2>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::mat2>(values);
			}

			uniform.value = uniform_value;
			uniform.transpose = transpose;
		}

		void Material::setFloatMat3(const char* name, const glm::mat3& value, bool transpose)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::mat3* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::mat3*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::mat3(value);
			}

			uniform.value = uniform_value;
			uniform.transpose = transpose;
		}

		void Material::setFloatMat3(const char* name, const std::vector<glm::mat3>& values, bool transpose)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::mat3>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::mat3>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::mat3>(values);
			}

			uniform.value = uniform_value;
			uniform.transpose = transpose;
		}

		void Material::setFloatMat4(const char* name, const glm::mat4& value, bool transpose)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::mat4* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::mat4*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::mat4(value);
			}

			uniform.value = uniform_value;
			uniform.transpose = transpose;
		}

		void Material::setFloatMat4(const char* name, const std::vector<glm::mat4>& values, bool transpose)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::mat4>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::mat4>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::mat4>(values);
			}

			uniform.value = uniform_value;
			uniform.transpose = transpose;
		}

		void Material::setDoubleMat2(const char* name, const glm::dmat2& value, bool transpose)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::dmat2* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::dmat2*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::dmat2(value);
			}

			uniform.value = uniform_value;
			uniform.transpose = transpose;
		}

		void Material::setDoubleMat2(const char* name, const std::vector<glm::dmat2>& values, bool transpose)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::dmat2>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::dmat2>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::dmat2>(values);
			}

			uniform.value = uniform_value;
			uniform.transpose = transpose;
		}

		void Material::setDoubleMat3(const char* name, const glm::dmat3& value, bool transpose)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::dmat3* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::dmat3*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::dmat3(value);
			}

			uniform.value = uniform_value;
			uniform.transpose = transpose;
		}

		void Material::setDoubleMat3(const char* name, const std::vector<glm::dmat3>& values, bool transpose)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::dmat3>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::dmat3>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::dmat3>(values);
			}

			uniform.value = uniform_value;
			uniform.transpose = transpose;
		}

		void Material::setDoubleMat4(const char* name, const glm::dmat4& value, bool transpose)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			glm::dmat4* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (glm::dmat4*)uniform.value;

				// set new value
				*uniform_value = value;
			}
			else
			{
				// allocate value
				uniform_value = new glm::dmat4(value);
			}

			uniform.value = uniform_value;
			uniform.transpose = transpose;
		}

		void Material::setDoubleMat4(const char* name, const std::vector<glm::dmat4>& values, bool transpose)
		{
			if (uniforms.count(name) == 0) return;

			// shader uniform
			Shader::Uniform& uniform = uniforms[name];

			// internal uniform value
			std::vector<glm::dmat4>* uniform_value = nullptr;

			// if we already have allocated value
			if (uniform.value)
			{
				// get pointer
				uniform_value = (std::vector<glm::dmat4>*)uniform.value;

				// set new value
				*uniform_value = values;
			}
			else
			{
				// allocate value
				uniform_value = new std::vector<glm::dmat4>(values);
			}

			uniform.value = uniform_value;
			uniform.transpose = transpose;
		}

		void Material::setTexture(const char* name, Texture* texture)
		{
			// set sampler index
			setInt(name, (int)textures.size());

			// store texture
			textures.push_back(texture);
		}

		void Material::setTextures(const std::vector<std::pair<std::string, Texture*>>& textures)
		{
			// clear textures
			this->textures.clear();

			// set textures
			for (auto& texture : textures)
				setTexture(texture.first.c_str(), texture.second);
		}
	}
}