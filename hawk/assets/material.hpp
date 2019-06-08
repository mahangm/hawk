#ifndef HAWK_ASSETS_MATERIAL_H
#define HAWK_ASSETS_MATERIAL_H

// stl
#include <vector>

// glm
#include <glm/glm.hpp>

// local
#include "asset.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace hawk
{
	namespace Assets
	{
		/**
		 * Material asset.
		 */
		class Material : public Asset
		{
		public:
			/**
			 * Type enum.
			 */
			enum class Type
			{
				None,
				Lit = 1,
				Unlit,
				Transparent
			};

			/**
			 * Constructor.
			 */
			Material();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Material(const Material& other);

			/**
			 * Constructor.
			 * @param shader Material shader.
			 * @param type Material type.
			 */
			Material(Shader* shader, Type type = Type::Unlit);

			/**
			 * Destructor.
			 */
			~Material();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			void operator=(const Material& other);

			/**
			 * Release resources.
			 */
			void release();

			/**
			 * Use material program.
			 */
			void use();

			/**
			 * Material shader.
			 */
			Shader* getShader() const;

			/**
			 * Material type.
			 */
			Type getMaterialType() const;

			/**
			 * Set render states.
			 */
			void setStates();

			/**
			 * Set active uniforms values.
			 */
			void setUniforms();

			/**
			 * Set texture samplers values.
			 */
			void setSamplers();

			/**
			 * Set a uniform bool variable.
			 */
			void setBool(const char* name, const int value);

			/**
			 * Set a list uniform bool variables.
			 */
			void setBool(const char* name, const std::vector<int>& values);

			/**
			 * Set a uniform bvec2 variable.
			 */
			void setBoolVec2(const char* name, const glm::ivec2& value);

			/**
			 * Set a list of uniform bvec2 variables.
			 */
			void setBoolVec2(const char* name, const std::vector<glm::ivec2>& values);

			/**
			 * Set a uniform bvec3 variable.
			 */
			void setBoolVec3(const char* name, const glm::ivec3& value);

			/**
			 * Set a list of uniform bvec3 variables.
			 */
			void setBoolVec3(const char* name, const std::vector<glm::ivec3>& values);

			/**
			 * Set a uniform bvec4 variable.
			 */
			void setBoolVec4(const char* name, const glm::ivec4& value);

			/**
			 * Set a list of uniform bvec4 variables.
			 */
			void setBoolVec4(const char* name, const std::vector<glm::ivec4>& values);

			/**
			 * Set a uniform int variable.
			 */
			void setInt(const char* name, const int value);

			/**
			 * Set a list uniform int variables.
			 */
			void setInt(const char* name, const std::vector<int>& values);

			/**
			 * Set a uniform ivec2 variable.
			 */
			void setIntVec2(const char* name, const glm::ivec2& value);

			/**
			 * Set a list of uniform ivec2 variables.
			 */
			void setIntVec2(const char* name, const std::vector<glm::ivec2>& values);

			/**
			 * Set a uniform ivec3 variable.
			 */
			void setIntVec3(const char* name, const glm::ivec3& value);

			/**
			 * Set a list of uniform ivec3 variables.
			 */
			void setIntVec3(const char* name, const std::vector<glm::ivec3>& values);

			/**
			 * Set a uniform ivec4 variable.
			 */
			void setIntVec4(const char* name, const glm::ivec4& value);

			/**
			 * Set a list of uniform ivec4 variables.
			 */
			void setIntVec4(const char* name, const std::vector<glm::ivec4>& values);

			/**
			 * Set a uniform uint variable.
			 */
			void setUInt(const char* name, const unsigned int value);

			/**
			 * Set a list uniform uint variables.
			 */
			void setUInt(const char* name, const std::vector<unsigned int>& values);

			/**
			 * Set a uniform uvec2 variable.
			 */
			void setUIntVec2(const char* name, const glm::uvec2& value);

			/**
			 * Set a list of uniform uvec2 variables.
			 */
			void setUIntVec2(const char* name, const std::vector<glm::uvec2>& values);

			/**
			 * Set a uniform uvec3 variable.
			 */
			void setUIntVec3(const char* name, const glm::uvec3& value);

			/**
			 * Set a list of uniform uvec3 variables.
			 */
			void setUIntVec3(const char* name, const std::vector<glm::uvec3>& values);

			/**
			 * Set a uniform uvec4 variable.
			 */
			void setUIntVec4(const char* name, const glm::uvec4& value);

			/**
			 * Set a list of uniform uvec4 variables.
			 */
			void setUIntVec4(const char* name, const std::vector<glm::uvec4>& values);

			/**
			 * Set a uniform float variable.
			 */
			void setFloat(const char* name, const float value);

			/**
			 * Set a list uniform float variables.
			 */
			void setFloat(const char* name, const std::vector<float>& values);

			/**
			 * Set a uniform fvec2 variable.
			 */
			void setFloatVec2(const char* name, const glm::vec2& value);

			/**
			 * Set a list of uniform fvec2 variables.
			 */
			void setFloatVec2(const char* name, const std::vector<glm::vec2>& values);

			/**
			 * Set a uniform fvec3 variable.
			 */
			void setFloatVec3(const char* name, const glm::vec3& value);

			/**
			 * Set a list of uniform fvec3 variables.
			 */
			void setFloatVec3(const char* name, const std::vector<glm::vec3>& values);

			/**
			 * Set a uniform fvec4 variable.
			 */
			void setFloatVec4(const char* name, const glm::vec4& value);

			/**
			 * Set a list of uniform fvec4 variables.
			 */
			void setFloatVec4(const char* name, const std::vector<glm::vec4>& values);

			/**
			 * Set a uniform double variable.
			 */
			void setDouble(const char* name, const double value);

			/**
			 * Set a list uniform double variables.
			 */
			void setDouble(const char* name, const std::vector<double>& values);

			/**
			 * Set a uniform dvec2 variable.
			 */
			void setDoubleVec2(const char* name, const glm::dvec2& value);

			/**
			 * Set a list of uniform dvec2 variables.
			 */
			void setDoubleVec2(const char* name, const std::vector<glm::dvec2>& values);

			/**
			 * Set a uniform dvec3 variable.
			 */
			void setDoubleVec3(const char* name, const glm::dvec3& value);

			/**
			 * Set a list of uniform dvec3 variables.
			 */
			void setDoubleVec3(const char* name, const std::vector<glm::dvec3>& values);

			/**
			 * Set a uniform dvec4 variable.
			 */
			void setDoubleVec4(const char* name, const glm::dvec4& value);

			/**
			 * Set a list of uniform dvec4 variables.
			 */
			void setDoubleVec4(const char* name, const std::vector<glm::dvec4>& values);

			/**
			 * Set a uniform mat2 variable.
			 */
			void setFloatMat2(const char* name, const glm::mat2& value, bool transpose = false);

			/**
			 * Set a list of uniform mat2 variables.
			 */
			void setFloatMat2(const char* name, const std::vector<glm::mat2>& values, bool transpose = false);

			/**
			 * Set a uniform mat3 variable.
			 */
			void setFloatMat3(const char* name, const glm::mat3& value, bool transpose = false);

			/**
			 * Set a list of uniform mat3 variables.
			 */
			void setFloatMat3(const char* name, const std::vector<glm::mat3>& values, bool transpose = false);

			/**
			 * Set a uniform mat4 variable.
			 */
			void setFloatMat4(const char* name, const glm::mat4& value, bool transpose = false);

			/**
			 * Set a list of uniform mat4 variables.
			 */
			void setFloatMat4(const char* name, const std::vector<glm::mat4>& values, bool transpose = false);

			/**
			 * Set a uniform dmat2 variable.
			 */
			void setDoubleMat2(const char* name, const glm::dmat2& value, bool transpose = false);

			/**
			 * Set a list of uniform dmat2 variables.
			 */
			void setDoubleMat2(const char* name, const std::vector<glm::dmat2>& values, bool transpose = false);

			/**
			 * Set a uniform dmat3 variable.
			 */
			void setDoubleMat3(const char* name, const glm::dmat3& value, bool transpose = false);

			/**
			 * Set a list of uniform dmat3 variables.
			 */
			void setDoubleMat3(const char* name, const std::vector<glm::dmat3>& values, bool transpose = false);

			/**
			 * Set a uniform dmat4 variable.
			 */
			void setDoubleMat4(const char* name, const glm::dmat4& value, bool transpose = false);

			/**
			 * Set a list of uniform dmat4 variables.
			 */
			void setDoubleMat4(const char* name, const std::vector<glm::dmat4>& values, bool transpose = false);

			/**
			 * Set a texture variable.
			 */
			void setTexture(const char* name, Texture* texture);

			/**
			 * Set a texture list.
			 */
			void setTextures(const std::vector<std::pair<std::string, Texture*>>& textures);

			/**
			 * Active uniforms list.
			 */
			std::map<std::string, Shader::Uniform> uniforms;

			/**
			 * List of textures.
			 */
			std::vector<Texture*> textures;

			/**
			 * Color mask. Write or ignore color values.
			 */
			bool color_mask;

			/**
			 * Depth test function.
			 */
			unsigned int depth_func;

			/**
			 * Depth test mask. Write or ignore depth values.
			 */
			bool depth_mask;

			/**
			 * Stencil test function.
			 */
			unsigned int stencil_func;

			/**
			 * Stencil test function reference value.
			 */
			int stencil_func_ref;

			/**
			 * Stencil test function mask value.
			 */
			unsigned int stencil_func_mask;

			/**
			 * Stencil test mask. Write or ignore stencil values.
			 */
			unsigned int stencil_mask;

			/**
			 * Blending function source factor.
			 */
			unsigned int blend_src_factor;

			/**
			 * Blending function destination factor.
			 */
			unsigned int blend_dst_factor;

			/**
			 * Blending equation.
			 */
			unsigned int blend_equation;

			/**
			 * Blending constant color.
			 */
			glm::vec4 blend_color;

			/**
			 * Face cull availability.
			 */
			bool face_cull;

			/**
			 * Face cull side.
			 */
			unsigned int face_side;

			/**
			 * Face cull wind direction.
			 */
			unsigned int face_orientation;

		private:
			/**
			 * Material shader.
			 */
			Shader* m_shader;

			/**
			 * Material type.
			 */
			Type m_material_type;
		};
	}
}
#endif