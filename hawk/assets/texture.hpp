#ifndef HAWK_ASSETS_TEXTURE_H
#define HAWK_ASSETS_TEXTURE_H

// glad
#include <glad/glad.h>

// local
#include "asset.hpp"

namespace hawk
{
	namespace Assets
	{
		/**
		 * Texture asset.
		 */
		class Texture : public Asset
		{
		public:
			/**
			 * Type enum.
			 */
			enum class Type
			{
				Opaque = 1,
				Transparent
			};

			/**
			 * Constructor.
			 */
			Texture();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Texture(const Texture& other);

			/**
			 * Destructor.
			 */
			~Texture();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Texture& other);

			/**
			 * Load an image from file.
			 * @param path Image file path.
			 * @param flip Flip image y axis.
			 * @param hdr Load data as floats.
			 * @param mips Minification maps count. Zero means generate automatically. Less than zero means no mips.
			 * @param internal_format Internal texture format. Zero means determine from image file.
			 * @parma data_format Texture data format. Zero means same as internal format.
			 * @param data_type Type of texture data. Zero means unsigned byte for non hdr and float for hdr images.
			 * @param wrap Texture wrapping when out of coordiantes.
			 * @param min_filter Texture filtering when getting away.
			 * @param mag_filter Texture filtering when getting close.
			 * @return True or False.
			 */
			bool load(const std::string& path, bool flip = true, bool hdr = false, int mips = 0, int internal_format = 0, unsigned int data_format = 0, unsigned int data_type = 0, int wrap = GL_REPEAT, int min_filter = GL_LINEAR_MIPMAP_LINEAR, int mag_filter = GL_LINEAR);

			/**
			 * Load a cube from file.
			 * @param paths Image file paths.
			 * @param flip Flip image y axis.
			 * @param hdr Load data as floats.
			 * @param mips Minification maps count. Zero means generate automatically. Less than zero means no mips.
			 * @param internal_format Internal texture format. Zero means determine from image file.
			 * @parma data_format Texture data format. Zero means same as internal format.
			 * @param data_type Type of texture data. Zero means unsigned byte for non hdr and float for hdr images.
			 * @param wrap Texture wrapping when out of coordiantes.
			 * @param min_filter Texture filtering when getting away.
			 * @param mag_filter Texture filtering when getting close.
			 * @return True or False.
			 */
			bool load(const std::vector<std::string>& paths, bool flip = true, bool hdr = false, int mips = 0, int internal_format = 0, unsigned int data_format = 0, unsigned int data_type = 0, int wrap = GL_REPEAT, int min_filter = GL_LINEAR_MIPMAP_LINEAR, int mag_filter = GL_LINEAR);

			/**
			 * Build a texture manually.
			 * @param width Texture width list.
			 * @param height Texture height list.
			 * @param data Texture data list.
			 * @param mips Minification maps count.
			 * @param target Texture bind target.
			 * @param internal_format Internal texture format.
			 * @param data_format Texture data format.
			 * @param data_type Type of texture data.
			 * @param wrap Texture wrapping when out of coordiantes.
			 * @param min_filter Texture filtering when getting away.
			 * @param mag_filter Texture filtering when getting close.
			 * @return True or False.
			 */
			bool build(const std::vector<int>& width, const std::vector<int>& height, const std::vector<const void*>& data, int mips, unsigned int target, int internal_format, unsigned int data_format, unsigned int data_type, int wrap, int min_filter, int mag_filter);

			/**
			 * Release resources.
			 */
			void release();

			/**
			 * Bind texture to graphics library.
			 * @param unit Texture unit.
			 */
			void bind(unsigned int unit = 0);

			/**
			 * Texture object.
			 */
			unsigned int getTexture() const;

			/**
			 * Image width.
			 */
			int getWidth() const;

			/**
			 * Image height.
			 */
			int getHeight() const;

			/**
			 * Texture bind target.
			 */
			unsigned int getTarget() const;

			/**
			 * Texture type.
			 */
			Type type;

			/**
			 * Sampler name in shader. Used for loading models.
			 */
			std::string sampler;

		private:
			/**
			 * Resolve formats.
			 * @param channels Color channels.
			 * @param hdr Handle data as hdr data.
			 * @param internal_format Internal texture format.
			 * @param data_format Texture data format.
			 * @param data_type Type of texture data.
			 */
			void format(int& channels, bool& hdr, int& internal_format, unsigned int& data_format, unsigned int& data_type);

			/**
			 * Texture object.
			 */
			unsigned int m_texture;

			/**
			 * Image width.
			 */
			int m_width;

			/**
			 * Image height.
			 */
			int m_height;

			/**
			 * Texture bind target.
			 */
			unsigned int m_target;
		};
	}
}
#endif