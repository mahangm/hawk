#include "../system/core.hpp"
#include "texture.hpp"
#include "../../libs/stb/stb_image.h"
#include <gli/gli.hpp>

namespace hawk
{
	namespace Assets
	{
		Texture::Texture() : type(Type::Opaque), m_texture(0), m_width(0), m_height(0), m_target(0)
		{
			// set asset type
			m_type = "Texture";
		}

		Texture::Texture(const Texture& other) : Asset(other), type(other.type), m_texture(other.m_texture),
			m_width(other.m_width), m_height(other.m_height), m_target(other.m_target)
		{
			// set asset type
			m_type = "Texture";
		}

		Texture::~Texture() {}

		void Texture::operator=(const Texture& other)
		{
			type = other.type;
			m_texture = other.m_texture;
			m_width = other.m_width;
			m_height = other.m_height;
			m_target = other.m_target;
		}

		bool Texture::load(const std::string& path, bool flip, bool hdr, int mips, int internal_format, unsigned int data_format, unsigned int data_type, int wrap, int min_filter, int mag_filter)
		{
			// application
			System::Core& app = System::Core::getInstance();

			// set asset path
			setPath(path);

			// flip y axis for libraries like OpenGL which use bottom left as origin for textures
			stbi_set_flip_vertically_on_load(flip);

			// texture width list
			std::vector<int> texture_width;

			// texture height list
			std::vector<int> texture_height;

			// texture data list
			std::vector<const void*> texture_data;

			// image byte data
			unsigned char* stbi_byte_data = nullptr;

			// image float data
			float* stbi_float_data = nullptr;

			// image mip data
			gli::texture gli_texture;

			// image channels
			int channels = 0;

			// load image with automatically generated mips
			if (mips <= 0 && path.rfind(".dds") == std::string::npos && path.rfind(".ktx") == std::string::npos)
			{
				// image size
				int width = 0, height = 0;

				// load image
				if (!hdr)
					stbi_byte_data = stbi_load(path.c_str(), &width, &height, &channels, 0);
				else
					stbi_float_data = stbi_loadf(path.c_str(), &width, &height, &channels, 0);

				// check data
				if (!stbi_byte_data && !stbi_float_data)
				{
					app.logger.error("Failed to load image!");
					app.logger.info(path.c_str());

					stbi_image_free(stbi_byte_data);
					stbi_image_free(stbi_float_data);

					return false;
				}

				// set texture size
				texture_width.push_back(width);
				texture_height.push_back(height);

				// set texture data
				if (!hdr) 
					texture_data.push_back(stbi_byte_data); 
				else 
					texture_data.push_back(stbi_float_data);
			}
			else
			{
				// load image
				gli_texture = gli::load(path);
				
				// check data
				if (gli_texture.empty())
				{
					app.logger.error("Failed to load image!");
					app.logger.info(path.c_str());

					return false;
				}

				// swizzles count
				int swizzles = 0;

				// set image channels
				if (gli_texture.swizzles().r == gli::swizzle::SWIZZLE_RED) swizzles++;
				if (gli_texture.swizzles().g == gli::swizzle::SWIZZLE_GREEN) swizzles++;
				if (gli_texture.swizzles().b == gli::swizzle::SWIZZLE_BLUE) swizzles++;
				if (gli_texture.swizzles().a == gli::swizzle::SWIZZLE_ALPHA) swizzles++;

				// set image channels
				channels = swizzles;

				// texture levels limit
				int levels = (mips > 0) ? mips : 1;

				// iterate levels
				for (std::size_t level = 0; level < levels; ++level)
				{
					// get image size
					glm::tvec3<int> extent = gli_texture.extent(level);

					// set texture size
					texture_width.push_back(extent.x);
					texture_height.push_back(extent.y);

					// set texture data
					texture_data.push_back(gli_texture.data(0, 0, level));
				}
			}

			// detected internal format
			int texture_internal_fromat = internal_format;

			// detected data format
			unsigned int texture_data_format = data_format;

			// detected data type
			unsigned int texture_data_type = data_type;

			// resolve formats
			format(channels, hdr, texture_internal_fromat, texture_data_format, texture_data_type);

			// build texture
			build(texture_width, texture_height, texture_data, mips, GL_TEXTURE_2D, texture_internal_fromat, texture_data_format, texture_data_type, wrap, min_filter, mag_filter);

			// free image data
			if (stbi_byte_data) stbi_image_free(stbi_byte_data);
			if (stbi_float_data) stbi_image_free(stbi_float_data);
			if (!gli_texture.empty()) gli_texture.clear();

			// clear list
			texture_data.clear();

			return true;
		}

		bool Texture::load(const std::vector<std::string>& paths, bool flip, bool hdr, int mips, int internal_format, unsigned int data_format, unsigned int data_type, int wrap, int min_filter, int mag_filter)
		{
			// application
			System::Core& app = System::Core::getInstance();

			// set asset path
			setPath(paths[0]);

			// flip y axis for libraries like OpenGL which use bottom left as origin for textures
			stbi_set_flip_vertically_on_load(flip);

			// texture width list
			std::vector<int> texture_width;

			// texture height list
			std::vector<int> texture_height;

			// texture data list
			std::vector<const void*> texture_data;

			// image byte data
			std::vector<unsigned char*> stbi_byte_data;

			// image float data
			std::vector<float*> stbi_float_data;

			// image mip data
			std::vector<gli::texture> gli_texture;

			// image channels
			int channels = 0;

			// iterate paths
			for (int i = 0; i < 6; ++i)
			{
				// image path
				const std::string& path = paths[i];

				// load image with automatically generated mips
				if (mips <= 0 && path.rfind(".dds") == std::string::npos && path.rfind(".ktx") == std::string::npos)
				{
					// image size
					int width = 0, height = 0;

					// load image
					if (!hdr)
						stbi_byte_data.push_back(stbi_load(path.c_str(), &width, &height, &channels, 0));
					else
						stbi_float_data.push_back(stbi_loadf(path.c_str(), &width, &height, &channels, 0));

					// check data
					if (!stbi_byte_data[i] && !stbi_float_data[i])
					{
						app.logger.error("Failed to load image!");
						app.logger.info(path.c_str());

						for (auto data : stbi_byte_data)
							stbi_image_free(data);

						for (auto data : stbi_float_data)
							stbi_image_free(data);

						return false;
					}

					// set texture size
					texture_width.push_back(width);
					texture_height.push_back(height);

					// set texture data
					if (!hdr)
						texture_data.push_back(stbi_byte_data[i]);
					else
						texture_data.push_back(stbi_float_data[i]);
				}
				else
				{
					// load image
					gli_texture.push_back(gli::load(path));

					// check data
					if (gli_texture[i].empty())
					{
						app.logger.error("Failed to load image!");
						app.logger.info(path.c_str());

						for (auto& data : gli_texture)
							if (!data.empty()) data.clear();

						return false;
					}

					// swizzles count
					int swizzles = 0;

					// set image channels
					if (gli_texture[i].swizzles().r == gli::swizzle::SWIZZLE_RED) swizzles++;
					if (gli_texture[i].swizzles().g == gli::swizzle::SWIZZLE_GREEN) swizzles++;
					if (gli_texture[i].swizzles().b == gli::swizzle::SWIZZLE_BLUE) swizzles++;
					if (gli_texture[i].swizzles().a == gli::swizzle::SWIZZLE_ALPHA) swizzles++;

					// set image channels
					channels = swizzles;

					// texture levels limit
					int levels = (mips > 0) ? mips : 1;

					// iterate levels
					for (std::size_t level = 0; level < levels; ++level)
					{
						// get image size
						glm::tvec3<int> extent = gli_texture[i].extent(level);

						// set texture size
						texture_width.push_back(extent.x);
						texture_height.push_back(extent.y);

						// set texture data
						texture_data.push_back(gli_texture[i].data(0, 0, level));
					}
				}
			}

			// detected internal format
			int texture_internal_fromat = internal_format;

			// detected data format
			unsigned int texture_data_format = data_format;

			// detected data type
			unsigned int texture_data_type = data_type;

			// resolve formats
			format(channels, hdr, texture_internal_fromat, texture_data_format, texture_data_type);

			// build texture
			build(texture_width, texture_height, texture_data, mips, GL_TEXTURE_CUBE_MAP, texture_internal_fromat, texture_data_format, texture_data_type, wrap, min_filter, mag_filter);

			// free image data
			for (auto data : stbi_byte_data)
				stbi_image_free(data);

			for (auto data : stbi_float_data)
				stbi_image_free(data);

			for (auto& data : gli_texture)
				if (!data.empty()) data.clear();

			// clear list
			texture_data.clear();

			return true;
		}

		bool Texture::build(const std::vector<int>& width, const std::vector<int>& height, const std::vector<const void*>& data, int mips, unsigned int target, int internal_format, unsigned int data_format, unsigned int data_type, int wrap, int min_filter, int mag_filter)
		{
			// generate texture
			glGenTextures(1, &m_texture);

			// bind texture
			glBindTexture(target, m_texture);

			// set levels
			if (mips > 0)
			{
				glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
				glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, mips - 1);
			}

			// set wrapping
			glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
			glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);
			glTexParameteri(target, GL_TEXTURE_WRAP_R, wrap);

			// set filtering
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, min_filter);
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, mag_filter);
			
			// make texture
			if (target == GL_TEXTURE_1D)
			{
				// build texture with automatically generated mipmaps
				if (mips <= 0)
				{
					// build texture
					glTexImage1D(target, 0, internal_format, width[0], 0, data_format, data_type, data[0]);

					// generate mipmap
					if (mips == 0) glGenerateMipmap(target);
				}
				else
				{
					// build texture for each mip
					for (int i = 0; i < mips; i++)
						glTexImage1D(target, i, internal_format, width[i], 0, data_format, data_type, data[i]);
				}
			}
			else if (target == GL_TEXTURE_2D)
			{
				// build texture with automatically generated mipmaps
				if (mips <= 0)
				{
					// build texture
					glTexImage2D(target, 0, internal_format, width[0], height[0], 0, data_format, data_type, data[0]);
					
					// generate mipmap
					if (mips == 0) glGenerateMipmap(target);
				}
				else
				{
					// build texture for each mip
					for (int i = 0; i < mips; ++i)
						glTexImage2D(target, i, internal_format, width[i], height[i], 0, data_format, data_type, data[i]);
				}
			}
			else if (target == GL_TEXTURE_CUBE_MAP)
			{
				// iterate cube map faces
				for (int i = 0; i < 6; ++i)
				{
					// build without mipmaps
					if (mips <= 0)
					{
						// build cube map face
						glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internal_format, width[0], height[0], 0, data_format, data_type, data[i]);
					}
					else
					{
						// build cube map face with mips
						for (int j = 0; j < mips; ++j)
							glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, j, internal_format, width[i * mips + j], height[i * mips + j], 0, data_format, data_type, data[i * mips + j]);
					}
				}
			}
			
			// unbind texture
			glBindTexture(target, 0);

			// set properties
			m_width = width[0];
			m_height = height[0];
			m_target = target;

			return true;
		}

		void Texture::release()
		{
			// free texture
			if (m_texture)
			{
				glDeleteTextures(1, &m_texture);

				m_texture = 0;
			}

			// free object
			delete this;
		}

		void Texture::bind(unsigned int unit)
		{
			// activate texture
			glActiveTexture(GL_TEXTURE0 + unit);

			// bind texture
			glBindTexture(m_target, m_texture);
		}

		unsigned int Texture::getTexture() const
		{
			return m_texture;
		}

		int Texture::getWidth() const
		{
			return m_width;
		}

		int Texture::getHeight() const
		{
			return m_height;
		}

		unsigned int Texture::getTarget() const
		{
			return m_target;
		}

		void Texture::format(int& channels, bool& hdr, int& internal_format, unsigned int& data_format, unsigned int& data_type)
		{
			// set texture data format from image channels if left at zero
			if (data_format == 0)
			{
				switch (channels)
				{
				case 1:
					data_format = GL_RED;
					break;
				case 2:
					data_format = GL_RG;
					break;
				case 3:
					data_format = GL_RGB;
					break;
				case 4:
					data_format = GL_RGBA;
					break;
				}
			}

			// set texture internal format
			if (internal_format == 0)
			{
				internal_format = data_format;
			}
			else if (internal_format == 1)
			{
				switch (data_format)
				{
				case GL_RGB:
					internal_format = GL_SRGB;
					break;
				case GL_RGBA:
					internal_format = GL_SRGB_ALPHA;
					break;
				}
			}
			
			// set data type
			if (data_type == 0) data_type = (!hdr) ? GL_UNSIGNED_BYTE : GL_FLOAT;
		}
	}
}