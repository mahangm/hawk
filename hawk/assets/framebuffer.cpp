#include "../system/core.hpp"
#include "framebuffer.hpp"

namespace hawk
{
	namespace Assets
	{
		Framebuffer::Framebuffer() : m_fbo(0), m_color_texture(nullptr), m_depth_stencil_texture(0), m_width(0), m_height(0), m_target(0) {}

		Framebuffer::Framebuffer(const Framebuffer& other) : m_fbo(other.m_fbo), m_color_texture(other.m_color_texture), 
			m_depth_stencil_texture(other.m_depth_stencil_texture), m_color_formats(other.m_color_formats),
			m_width(other.m_width), m_height(other.m_height), m_target(other.m_target) {}

		Framebuffer::~Framebuffer() {}

		void Framebuffer::operator=(const Framebuffer& other) 
		{
			m_fbo = other.m_fbo;
			m_color_texture = other.m_color_texture;
			m_depth_stencil_texture = other.m_depth_stencil_texture;
			m_color_formats = other.m_color_formats;
			m_width = other.m_width;
			m_height = other.m_height;
			m_target = other.m_target;
		}

		bool Framebuffer::build(int width, int height, unsigned int target,
			int wrap, int filter, const glm::vec4& border_color, int attachments, 
			const std::vector<BufferFormat>& color_formats)
		{
			// application
			System::Core& app = System::Core::getInstance();

			// border color float vector
			float border_color_fv[] = { border_color.r, border_color.g, border_color.b, border_color.a };

			// color bit flag
			bool has_color = (attachments & Attachment::Color) == Attachment::Color;

			// depth bit flag
			bool has_depth = (attachments & Attachment::Depth) == Attachment::Depth;

			// stencil bit flag
			bool has_stencil = (attachments & Attachment::Stencil) == Attachment::Stencil;

			// return if stencil attachment is included without depth attachment
			if (has_stencil && !has_depth)
			{
				app.logger.error("Failed to build frame buffer!");
				app.logger.info("Can't have separate stencil attachment.");

				return false;
			}

			// make frame buffer
			glGenFramebuffers(1, &m_fbo);

			// bind frame buffer
			glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

			// if we have color attachment
			if (has_color)
			{
				// make color buffers
				m_color_texture = new unsigned int[color_formats.size()];

				// make color attachments list
				unsigned int* color_attachments = new unsigned int[color_formats.size()];

				// generate color attachment
				glGenTextures((int)color_formats.size(), m_color_texture);

				// make color textures
				for (unsigned int i = 0; i < color_formats.size(); ++i)
				{
					// set color attachment
					color_attachments[i] = GL_COLOR_ATTACHMENT0 + i;

					// bind texture
					glBindTexture(target, m_color_texture[i]);

					// set wrapping
					glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
					glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);

					// set filtering
					glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
					glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);

					// set border color if wrap is clamp to border
					if (wrap == GL_CLAMP_TO_BORDER)
						glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, border_color_fv);

					// build texture
					switch (target)
					{
					case GL_TEXTURE_2D:
						// generate color attachment
						glTexImage2D(target, 0, color_formats[i].internal_format, width, height, 0, color_formats[i].data_format, color_formats[i].data_type, nullptr);

						// bind to frame buffer
						glFramebufferTexture2D(GL_FRAMEBUFFER, color_attachments[i], target, m_color_texture[i], 0);
						break;
					case GL_TEXTURE_CUBE_MAP:
						// generate color attachment
						for (int j = 0; j < 6; ++j)
							glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, color_formats[i].internal_format, width, height, 0, color_formats[i].data_format, color_formats[i].data_type, nullptr);

						// bind to frame buffer
						glFramebufferTexture(GL_FRAMEBUFFER, color_attachments[i], m_color_texture[i], 0);
						break;
					}
				}

				// explicitly tell opengl about multiple color buffers
				if (color_formats.size() > 1) glDrawBuffers((int)color_formats.size(), color_attachments);

				// free color attachments list
				delete[] color_attachments;

				// unbind texture
				glBindTexture(target, 0);
			}
			else
			{
				// disable color read/write buffers
				glReadBuffer(GL_NONE);
				glDrawBuffer(GL_NONE);
			}

			// if we have depth or stencil attachment
			if (has_depth || has_stencil)
			{
				// generate depth stencil attachment
				glGenTextures(1, &m_depth_stencil_texture);

				// bind texture
				glBindTexture(target, m_depth_stencil_texture);

				// set wrapping
				glTexParameteri(target, GL_TEXTURE_WRAP_S, wrap);
				glTexParameteri(target, GL_TEXTURE_WRAP_T, wrap);

				// set filtering
				glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filter);
				glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filter);

				// set border color if wrap is clamp to border
				if (wrap == GL_CLAMP_TO_BORDER)
					glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, border_color_fv);

				// if we only have depth attachment
				if (has_depth && !has_stencil)
				{
					// build texture
					switch (target)
					{
					case GL_TEXTURE_2D:
						// generate depth attachment
						glTexImage2D(target, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

						// bind to frame buffer
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, m_depth_stencil_texture, 0);
						break;
					case GL_TEXTURE_CUBE_MAP:
						// generate depth attachment
						for (int i = 0; i < 6; ++i)
							glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

						// bind to frame buffer
						glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth_stencil_texture, 0);
						break;
					}
				}
				else if (has_depth && has_stencil)
				{
					// build texture
					switch (target)
					{
					case GL_TEXTURE_2D:
						// generate depth stencil attachment
						glTexImage2D(target, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

						// bind to frame buffer
						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, target, m_depth_stencil_texture, 0);
						break;
					case GL_TEXTURE_CUBE_MAP:
						// generate depth stencil attachment
						for (int i = 0; i < 6; ++i)
							glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

						// bind to frame buffer
						glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, m_depth_stencil_texture, 0);
						break;
					}
				}
				
				// unbind texture
				glBindTexture(target, 0);
			}

			// set color format
			m_color_formats = color_formats;

			// check frame buffer status
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				app.logger.error("Framebuffer incomplete!");

				// unbind frame buffer
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				return false;
			}

			// unbind frame buffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// set properties
			m_width = width;
			m_height = height;
			m_target = target;

			return true;
		}

		void Framebuffer::blit(const Framebuffer* src, const Framebuffer* dst, 
			int src_x, int src_y, int src_w, int src_h, 
			int dst_x, int dst_y, int dst_w, int dst_h, 
			unsigned int bits, 
			unsigned int filter)
		{
			// copy color buffer flag
			bool copy_color_buffer = (bits & GL_COLOR_BUFFER_BIT) == GL_COLOR_BUFFER_BIT;

			// copy depth buffer flag
			bool copy_depth_buffer = (bits & GL_DEPTH_BUFFER_BIT) == GL_DEPTH_BUFFER_BIT;

			// copy stencil buffer flag
			bool copy_stencil_buffer = (bits & GL_STENCIL_BUFFER_BIT) == GL_STENCIL_BUFFER_BIT;

			// bind source frame buffer as read buffer
			glBindFramebuffer(GL_READ_FRAMEBUFFER, src->getFbo());

			// bind destination frame buffer as write buffer
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->getFbo());

			// copy buffers
			if (copy_color_buffer)
			{
				// iterate color buffers
				for (unsigned int i = 0; i < src->getColorFormats().size(); ++i)
				{
					// set read buffer
					glReadBuffer(GL_COLOR_ATTACHMENT0 + i);

					// set draw buffer
					glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);

					// copy buffers
					glBlitFramebuffer(src_x, src_y, src_w, src_h, dst_x, dst_y, dst_w, dst_h, bits, filter);
				}
			}
			else if (copy_depth_buffer || copy_stencil_buffer)
			{
				glBlitFramebuffer(src_x, src_y, src_w, src_h, dst_x, dst_y, dst_w, dst_h, bits, filter);
			}

			// unbind read/write buffers
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void Framebuffer::release()
		{
			// free frame buffer
			if (m_fbo)
			{
				glDeleteFramebuffers(1, &m_fbo);

				m_fbo = 0;
			}

			// free color attachments
			if (m_color_texture)
			{
				glDeleteTextures((int)m_color_formats.size(), m_color_texture);

				delete[] m_color_texture;

				m_color_texture = nullptr;
			}

			// free depth stencil attachment
			if (m_depth_stencil_texture)
			{
				glDeleteTextures(1, &m_depth_stencil_texture);

				m_depth_stencil_texture = 0;
			}

			// free object
			delete this;
		}

		void Framebuffer::bind()
		{
			// bind frame buffer
			glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

			// set viewport
			glViewport(0, 0, m_width, m_height);
		}

		void Framebuffer::bindColorTexture(unsigned int index, unsigned int unit)
		{
			// activate texture
			glActiveTexture(GL_TEXTURE0 + unit);

			// bind texture
			glBindTexture(m_target, m_color_texture[index]);
		}

		void Framebuffer::bindDepthStencilTexture(unsigned int unit)
		{
			// activate texture
			glActiveTexture(GL_TEXTURE0 + unit);

			// bind texture
			glBindTexture(m_target, m_depth_stencil_texture);
		}

		void Framebuffer::clear(const glm::vec4& color, unsigned int bits)
		{
			// set clear color
			glClearColor(color.r, color.g, color.b, color.a);

			// clear bound buffers
			glClear(bits);
		}

		unsigned int Framebuffer::getFbo() const
		{
			return m_fbo;
		}

		unsigned int* Framebuffer::getColorTextures() const
		{
			return m_color_texture;
		}

		unsigned int Framebuffer::getDepthStencilTexture() const
		{
			return m_depth_stencil_texture;
		}

		const std::vector<Framebuffer::BufferFormat>& Framebuffer::getColorFormats() const
		{
			return m_color_formats;
		}

		int Framebuffer::getWidth() const
		{
			return m_width;
		}

		int Framebuffer::getHeight() const
		{
			return m_height;
		}

		unsigned int Framebuffer::getTarget() const
		{
			return m_target;
		}
	}
}