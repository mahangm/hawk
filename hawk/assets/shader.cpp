#include "../system/core.hpp"
#include "shader.hpp"
#include "../helpers/file_system.hpp"
#include "../helpers/strings.hpp"
#include <glad/glad.h>
#include <regex>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

namespace hawk
{
	namespace Assets
	{
		Shader::Shader() : m_program(0)
		{
			// set asset type
			m_type = "Shader";
		}

		Shader::Shader(const Shader& other) : Asset(other), m_program(other.m_program), m_uniforms(other.m_uniforms)
		{
			// set asset type
			m_type = "Shader";
		}

		Shader::~Shader() {}

		void Shader::operator=(const Shader& other)
		{
			m_program = other.m_program;
			m_uniforms = other.m_uniforms;
		}

		bool Shader::load(const std::vector<std::string>& paths)
		{
			// application
			System::Core& app = System::Core::getInstance();

			// set asset path
			setPath(paths[0]);

			// shader source
			std::vector<std::string> sources;

			// add vertex shader slot
			sources.push_back(std::string());

			// read vertex shader file
			if (!hawk::Helpers::FileSystem::readTextFile(paths[0].c_str(), sources[0]))
			{
				app.logger.error("Failed to read shader source file!");
				app.logger.info(paths[0].c_str());

				return false;
			}

			// add fragment shader slot
			sources.push_back(std::string());

			// read fragment shader file
			if (!hawk::Helpers::FileSystem::readTextFile(paths[1].c_str(), sources[1]))
			{
				app.logger.error("Failed to read shader source file!");
				app.logger.info(paths[1].c_str());

				return false;
			}

			// if we have geometry shader
			if (paths.size() > 2 && !paths[2].empty())
			{
				// add geometry shader slot
				sources.push_back(std::string());

				// read geometry shader file
				if (!hawk::Helpers::FileSystem::readTextFile(paths[2].c_str(), sources[2]))
				{
					app.logger.error("Failed to read shader source file!");
					app.logger.info(paths[2].c_str());

					return false;
				}
			}

			// parse sources
			for (int i = 0; i < sources.size(); ++i)
			{
				// parse shaders
				if (!parse(sources[i], Helpers::FileSystem::getDirectory(paths[i])))
				{
					app.logger.error("Failed to parse shader include files!");
					app.logger.info(m_name.c_str());

					return false;
				}
			}

			// build shader
			if (!build(sources))
			{
				app.logger.error("Failed to build shader!");

				return false;
			}

			return true;
		}

		bool Shader::build(const std::vector<std::string>& sources)
		{
			// application
			System::Core& app = System::Core::getInstance();

			// list of shader objects
			std::vector<unsigned int> shaders;

			// iterate sources
			for (int i = 0; i < sources.size(); ++i)
			{
				// shader type
				const char* type = "unknown";

				// make vertex shader object
				switch (i)
				{
				case 0:
					type = "vertex";

					shaders.push_back(glCreateShader(GL_VERTEX_SHADER));
					break;
				case 1:
					type = "fragment";

					shaders.push_back(glCreateShader(GL_FRAGMENT_SHADER));
					break;
				case 2:
					type = "geometry";

					shaders.push_back(glCreateShader(GL_GEOMETRY_SHADER));
					break;
				}

				// compile vertex shader
				if (!compile(shaders[i], sources[i].c_str()))
				{
					// log text
					char log[64] = { 0 };

					std::snprintf(log, sizeof log, "Failed to compile %s shader!", type);

					app.logger.error(log);
					app.logger.info(m_name.c_str());

					// free shaders
					for (auto shader : shaders)
						glDeleteShader(shader);

					shaders.clear();

					return false;
				}
			}

			// make shader program
			m_program = glCreateProgram();

			// link shaders
			if (!link(m_program, shaders))
			{
				app.logger.error("Failed to link shaders!");
				app.logger.info(m_name.c_str());

				// free shader program
				if (m_program)
				{
					glDeleteProgram(m_program);

					m_program = 0;
				}

				return false;
			}

			// free shaders
			for (auto shader : shaders)
				glDeleteShader(shader);

			shaders.clear();

			// clear uniforms
			m_uniforms.clear();

			// active uniforms count in shader program
			int active_uniforms_count = 0;

			// get active uniforms count
			glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &active_uniforms_count);

			// if we have active uniforms
			if (active_uniforms_count > 0)
			{
				// iterate active uniforms
				for (int i = 0; i < active_uniforms_count; ++i)
				{
					// shader uniform
					Uniform uniform{ "", 1, 0, -1, nullptr, false };

					// get uniform
					glGetActiveUniform(m_program, i, sizeof uniform.name, nullptr, &uniform.size, &uniform.type, uniform.name);

					// skip uniform buffers
					if (std::strncmp(uniform.name, "Camera.", 7) == 0 ||
						std::strncmp(uniform.name, "Scene.", 6) == 0 ||
						std::strncmp(uniform.name, "Video.", 6) == 0) continue;

					// remove indexer bracket if this is an array from name, weird because uniform location is the same wether we get it with indexer brackets or not
					if (uniform.size > 1) uniform.name[std::strchr(uniform.name, '[') - uniform.name] = 0;

					// get uniform location
					uniform.location = glGetUniformLocation(m_program, uniform.name);

					// store uniform
					m_uniforms[uniform.name] = uniform;
				}
			}

			return true;
		}

		void Shader::release()
		{
			// free shader program
			if (m_program)
			{
				glDeleteProgram(m_program);

				m_program = 0;
			}

			// free object
			delete this;
		}

		unsigned int Shader::getProgram() const
		{
			return m_program;
		}

		const std::map<std::string, Shader::Uniform>& Shader::getUniforms() const
		{
			return m_uniforms;
		}

		void Shader::setBool(const char* name, const int value)
		{
			setInt(name, value);
		}

		void Shader::setBool(const char* name, const std::vector<int>& values)
		{
			setInt(name, values);
		}

		void Shader::setBoolVec2(const char* name, const glm::ivec2& value)
		{
			setIntVec2(name, value);
		}

		void Shader::setBoolVec2(const char* name, const std::vector<glm::ivec2>& values)
		{
			setIntVec2(name, values);
		}

		void Shader::setBoolVec3(const char* name, const glm::ivec3& value)
		{
			setIntVec3(name, value);
		}

		void Shader::setBoolVec3(const char* name, const std::vector<glm::ivec3>& values)
		{
			setIntVec3(name, values);
		}

		void Shader::setBoolVec4(const char* name, const glm::ivec4& value)
		{
			setIntVec4(name, value);
		}

		void Shader::setBoolVec4(const char* name, const std::vector<glm::ivec4>& values)
		{
			setIntVec4(name, values);
		}

		void Shader::setInt(const char* name, const int value)
		{
			glProgramUniform1i(m_program, glGetUniformLocation(m_program, name), value);
		}

		void Shader::setInt(const char* name, const std::vector<int>& values)
		{
			glProgramUniform1iv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), &values[0]);
		}

		void Shader::setIntVec2(const char* name, const glm::ivec2& value)
		{
			glProgramUniform2i(m_program, glGetUniformLocation(m_program, name), value.x, value.y);
		}

		void Shader::setIntVec2(const char* name, const std::vector<glm::ivec2>& values)
		{
			glProgramUniform2iv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), glm::value_ptr(values[0]));
		}

		void Shader::setIntVec3(const char* name, const glm::ivec3& value)
		{
			glProgramUniform3i(m_program, glGetUniformLocation(m_program, name), value.x, value.y, value.z);
		}

		void Shader::setIntVec3(const char* name, const std::vector<glm::ivec3>& values)
		{
			glProgramUniform3iv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), glm::value_ptr(values[0]));
		}

		void Shader::setIntVec4(const char* name, const glm::ivec4& value)
		{
			glProgramUniform4i(m_program, glGetUniformLocation(m_program, name), value.x, value.y, value.z, value.w);
		}

		void Shader::setIntVec4(const char* name, const std::vector<glm::ivec4>& values)
		{
			glProgramUniform4iv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), glm::value_ptr(values[0]));
		}

		void Shader::setUInt(const char* name, const unsigned int value)
		{
			glProgramUniform1ui(m_program, glGetUniformLocation(m_program, name), value);
		}

		void Shader::setUInt(const char* name, const std::vector<unsigned int>& values)
		{
			glProgramUniform1uiv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), &values[0]);
		}

		void Shader::setUIntVec2(const char* name, const glm::uvec2& value)
		{
			glProgramUniform2ui(m_program, glGetUniformLocation(m_program, name), value.x, value.y);
		}

		void Shader::setUIntVec2(const char* name, const std::vector<glm::uvec2>& values)
		{
			glProgramUniform2uiv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), glm::value_ptr(values[0]));
		}

		void Shader::setUIntVec3(const char* name, const glm::uvec3& value)
		{
			glProgramUniform3ui(m_program, glGetUniformLocation(m_program, name), value.x, value.y, value.z);
		}

		void Shader::setUIntVec3(const char* name, const std::vector<glm::uvec3>& values)
		{
			glProgramUniform3uiv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), glm::value_ptr(values[0]));
		}

		void Shader::setUIntVec4(const char* name, const glm::uvec4& value)
		{
			glProgramUniform4ui(m_program, glGetUniformLocation(m_program, name), value.x, value.y, value.z, value.w);
		}

		void Shader::setUIntVec4(const char* name, const std::vector<glm::uvec4>& values)
		{
			glProgramUniform4uiv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), glm::value_ptr(values[0]));
		}

		void Shader::setFloat(const char* name, const float value)
		{
			glProgramUniform1f(m_program, glGetUniformLocation(m_program, name), value);
		}

		void Shader::setFloat(const char* name, const std::vector<float>& values)
		{
			glProgramUniform1fv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), &values[0]);
		}

		void Shader::setFloatVec2(const char* name, const glm::vec2& value)
		{
			glProgramUniform2f(m_program, glGetUniformLocation(m_program, name), value.x, value.y);
		}

		void Shader::setFloatVec2(const char* name, const std::vector<glm::vec2>& values)
		{
			glProgramUniform2fv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), glm::value_ptr(values[0]));
		}

		void Shader::setFloatVec3(const char* name, const glm::vec3& value)
		{
			glProgramUniform3f(m_program, glGetUniformLocation(m_program, name), value.x, value.y, value.z);
		}

		void Shader::setFloatVec3(const char* name, const std::vector<glm::vec3>& values)
		{
			glProgramUniform3fv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), glm::value_ptr(values[0]));
		}

		void Shader::setFloatVec4(const char* name, const glm::vec4& value)
		{
			glProgramUniform4f(m_program, glGetUniformLocation(m_program, name), value.x, value.y, value.z, value.w);
		}

		void Shader::setFloatVec4(const char* name, const std::vector<glm::vec4>& values)
		{
			glProgramUniform4fv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), glm::value_ptr(values[0]));
		}

		void Shader::setDouble(const char* name, const double value)
		{
			glProgramUniform1d(m_program, glGetUniformLocation(m_program, name), value);
		}

		void Shader::setDouble(const char* name, const std::vector<double>& values)
		{
			glProgramUniform1dv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), &values[0]);
		}

		void Shader::setDoubleVec2(const char* name, const glm::dvec2& value)
		{
			glProgramUniform2d(m_program, glGetUniformLocation(m_program, name), value.x, value.y);
		}

		void Shader::setDoubleVec2(const char* name, const std::vector<glm::dvec2>& values)
		{
			glProgramUniform2dv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), glm::value_ptr(values[0]));
		}

		void Shader::setDoubleVec3(const char* name, const glm::dvec3& value)
		{
			glProgramUniform3d(m_program, glGetUniformLocation(m_program, name), value.x, value.y, value.z);
		}

		void Shader::setDoubleVec3(const char* name, const std::vector<glm::dvec3>& values)
		{
			glProgramUniform3dv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), glm::value_ptr(values[0]));
		}

		void Shader::setDoubleVec4(const char* name, const glm::dvec4& value)
		{
			glProgramUniform4d(m_program, glGetUniformLocation(m_program, name), value.x, value.y, value.z, value.w);
		}

		void Shader::setDoubleVec4(const char* name, const std::vector<glm::dvec4>& values)
		{
			glProgramUniform4dv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), glm::value_ptr(values[0]));
		}

		void Shader::setFloatMat2(const char* name, const glm::mat2& value, bool transpose)
		{
			glProgramUniformMatrix2fv(m_program, glGetUniformLocation(m_program, name), 1, transpose, glm::value_ptr(value));
		}

		void Shader::setFloatMat2(const char* name, const std::vector<glm::mat2>& values, bool transpose)
		{
			glProgramUniformMatrix2fv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), transpose, glm::value_ptr(values[0]));
		}

		void Shader::setFloatMat3(const char* name, const glm::mat3& value, bool transpose)
		{
			glProgramUniformMatrix3fv(m_program, glGetUniformLocation(m_program, name), 1, transpose, glm::value_ptr(value));
		}

		void Shader::setFloatMat3(const char* name, const std::vector<glm::mat3>& values, bool transpose)
		{
			glProgramUniformMatrix3fv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), transpose, glm::value_ptr(values[0]));
		}

		void Shader::setFloatMat4(const char* name, const glm::mat4& value, bool transpose)
		{
			glProgramUniformMatrix4fv(m_program, glGetUniformLocation(m_program, name), 1, transpose, glm::value_ptr(value));
		}

		void Shader::setFloatMat4(const char* name, const std::vector<glm::mat4>& values, bool transpose)
		{
			glProgramUniformMatrix4fv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), transpose, glm::value_ptr(values[0]));
		}

		void Shader::setDoubleMat2(const char* name, const glm::dmat2& value, bool transpose)
		{
			glProgramUniformMatrix2dv(m_program, glGetUniformLocation(m_program, name), 1, transpose, glm::value_ptr(value));
		}

		void Shader::setDoubleMat2(const char* name, const std::vector<glm::dmat2>& values, bool transpose)
		{
			glProgramUniformMatrix2dv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), transpose, glm::value_ptr(values[0]));
		}

		void Shader::setDoubleMat3(const char* name, const glm::dmat3& value, bool transpose)
		{
			glProgramUniformMatrix3dv(m_program, glGetUniformLocation(m_program, name), 1, transpose, glm::value_ptr(value));
		}

		void Shader::setDoubleMat3(const char* name, const std::vector<glm::dmat3>& values, bool transpose)
		{
			glProgramUniformMatrix3dv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), transpose, glm::value_ptr(values[0]));
		}

		void Shader::setDoubleMat4(const char* name, const glm::dmat4& value, bool transpose)
		{
			glProgramUniformMatrix4dv(m_program, glGetUniformLocation(m_program, name), 1, transpose, glm::value_ptr(value));
		}

		void Shader::setDoubleMat4(const char* name, const std::vector<glm::dmat4>& values, bool transpose)
		{
			glProgramUniformMatrix4dv(m_program, glGetUniformLocation(m_program, name), (int)values.size(), transpose, glm::value_ptr(values[0]));
		}

		bool Shader::compile(unsigned int& shader, const char* source)
		{
			// application
			System::Core& app = System::Core::getInstance();

			// set shader source
			glShaderSource(shader, 1, &source, nullptr);

			// compile
			glCompileShader(shader);

			// compilation status
			int result = 0;

			// get compilation status
			glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

			// if result is not ok
			if (!result)
			{
				// result message
				char message[512] = { 0 };

				// get result message
				glGetShaderInfoLog(shader, sizeof message, nullptr, message);

				// log result message
				app.logger.info(message);

				return false;
			}

			return true;
		}

		bool Shader::link(unsigned int program, const std::vector<unsigned int>& shaders)
		{
			// application
			System::Core& app = System::Core::getInstance();

			// attach shaders
			for (auto shader : shaders)
				glAttachShader(program, shader);

			// link shaders
			glLinkProgram(program);

			// link status
			int result = 0;

			// get link status
			glGetProgramiv(program, GL_LINK_STATUS, &result);

			// if result is not ok
			if (!result)
			{
				// result message
				char message[512] = { 0 };

				// get result message
				glGetProgramInfoLog(program, 512, nullptr, message);

				// log result message
				app.logger.info(message);

				return false;
			}

			return true;
		}

		bool Shader::parse(std::string& source, const std::string& base)
		{
			// parse includes
			if (!parseIncludes(source, base)) return false;

			// parse macros
			parseMacros(source);

			return true;
		}

		bool Shader::parseIncludes(std::string& source, const std::string& base)
		{
			// search pattern
			std::regex pattern("#include \"(.*)\"");

			// included files list
			std::vector<Include> includes;

			// search for include pattern
			for (auto match = std::sregex_iterator(source.begin(), source.end(), pattern); match != std::sregex_iterator(); ++match)
			{
				// included file name
				std::string file_name = match->str(1);

				// included file content
				std::string file_content;

				// read include file
				if (!Helpers::FileSystem::readTextFile(std::string(base + file_name).c_str(), file_content)) return false;

				// parse includes in newly read file
				if (!parseIncludes(file_content, Helpers::FileSystem::getDirectory(base + file_name))) return false;

				// push to the includes list
				includes.push_back({ match->position(), match->length(), file_content });
			}
			
			// if there are included files
			if (!includes.empty())
			{
				// shifted string position
				std::size_t shifted = 0;

				// replace includes
				for (auto& include : includes)
				{
					source.replace(include.position + shifted, include.length, include.content);

					// increase shifted position
					shifted += (include.content.length() > (size_t)include.length) ? include.content.length() - (size_t)include.length : include.content.length() - (size_t)include.length;
				}
			}

			return true;
		}

		void Shader::parseMacros(std::string& source)
		{
			// application
			System::Core& app = System::Core::getInstance();

			// replace constants placeholder
			Helpers::Strings::replace(source, "$$MACROS$$", app.video.getShadersMacros());
		}
	}
}