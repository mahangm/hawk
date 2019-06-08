#ifndef HAWK_ASSETS_FRAMEBUFFER_H
#define HAWK_ASSETS_FRAMEBUFFER_H

// glad
#include <glad/glad.h>

// glm
#include <glm/glm.hpp>

namespace hawk
{
	namespace Assets
	{
		/**
		 * Frame buffer wrapper.
		 */
		class Framebuffer
		{
		public:
			/**
			 * Attachment enum.
			 */
			enum Attachment
			{
				Color = 1 << 0,
				Depth = 1 << 1,
				Stencil = 1 << 2
			};

			/**
			 * Buffer format struct.
			 */
			struct BufferFormat
			{
				int internal_format;
				int data_format;
				unsigned int data_type;
			};

			/**
			 * Constructor.
			 */
			Framebuffer();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Framebuffer(const Framebuffer& other);

			/**
			 * Destructor.
			 */
			~Framebuffer();
			
			/**
			 * Constructor.
			 * @param other Other object.
			 */
			void operator=(const Framebuffer& other);

			/**
			 * Generate frame buffer. 
			 * Notes: You can't have separate stencil attachment, it should come with depth attachment.
			 * @param width Frame buffer width.
			 * @param height Frame buffer height.
			 * @param target Frame buffer bind target.
			 * @param wrap Attachments wrap.
			 * @param filter Attachments filter.
			 * @param border_color Attachments border color.
			 * @param attachments Attachments flags from Attachment enum.
			 * @param color_count Color attachment count for multiple color buffers.
			 * @return True or False.
			 */
			bool build(int width, int height, unsigned int target,
				int wrap = GL_CLAMP_TO_EDGE, int filter = GL_LINEAR, const glm::vec4& border_color = glm::vec4(1.0f), 
				int attachments = Attachment::Color | Attachment::Depth | Attachment::Stencil, 
				const std::vector<BufferFormat>& color_formats = std::vector<BufferFormat> ());

			/**
			 * Copy and resolve frame buffers.
			 * @param src Source frame buffer.
			 * @param dst Destination frame buffer.
			 * @param src_x Source frame buffer left.
			 * @param src_y Source frame buffer top.
			 * @param src_w Source frame buffer width.
			 * @param src_h Source frame buffer height.
			 * @param dst_x Destination frame buffer left.
			 * @param dst_y Destination frame buffer top.
			 * @param dst_w Destination frame buffer width.
			 * @param dst_h Destination frame buffer height.
			 * @param bits Frame buffer bits to copy.
			 * @param filter Frame buffer filter.
			 */
			static void blit(const Framebuffer* src, const Framebuffer* dst,
				int src_x, int src_y, int src_w, int src_h,
				int dst_x, int dst_y, int dst_w, int dst_h,
				unsigned int bits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
				unsigned int filter = GL_NEAREST);

			/**
			 * Release resources.
			 */
			void release();

			/**
			 * Bind frame buffer.
			 */
			void bind();

			/**
			 * Bind color texture to graphics library.
			 * @param index Texture index.
			 * @param unit Texture unit.
			 */
			void bindColorTexture(unsigned int index = 0, unsigned int unit = 0);

			/**
			 * Bind depth stencil texture to graphics library.
			 * @param unit Texture unit.
			 */
			void bindDepthStencilTexture(unsigned int unit = 0);

			/**
			 * Clear bound buffers.
			 * @param color Clear color.
			 * @param bits Buffer bits.
			 */
			void clear(const glm::vec4& color = glm::vec4(0.0f), unsigned int bits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			/**
			 * Frame buffer object.
			 */
			unsigned int getFbo() const;

			/**
			 * Color attachments.
			 */
			unsigned int* getColorTextures() const;

			/**
			 * Septh stencil attachment.
			 */
			unsigned int getDepthStencilTexture() const;

			/**
			 * List of color textures format.
			 */
			const std::vector<BufferFormat>& getColorFormats() const;

			/**
			 * Frame buffer width.
			 */
			int getWidth() const;

			/**
			 * Frame buffer height.
			 */
			int getHeight() const;

			/**
			 * Frame buffer texture target.
			 */
			unsigned int getTarget() const;

		private:
			/**
			 * Frame buffer object.
			 */
			unsigned int m_fbo;

			/**
			 * Color attachments.
			 */
			unsigned int* m_color_texture;

			/**
			 * Depth stencil attachment.
			 */
			unsigned int m_depth_stencil_texture;

			/**
			 * List of color textures format.
			 */
			std::vector<BufferFormat> m_color_formats;

			/**
			 * Frame buffer width.
			 */
			int m_width;

			/**
			 * Frame buffer height.
			 */
			int m_height;

			/**
			 * Frame buffer bind target.
			 */
			unsigned int m_target;
		};
	}
}

#endif