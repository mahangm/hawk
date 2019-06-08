#ifndef HAWK_MANAGERS_VIDEO_H
#define HAWK_MANAGERS_VIDEO_H

// stl
#include <vector>

// glad
#include <glad/glad.h>

// glfw
#include <GLFW/glfw3.h>

// local
#include "manager.hpp"
#include "../assets/material.hpp"
#include "../assets/mesh.hpp"
#include "../assets/framebuffer.hpp"
#include "../components/camera.hpp"
#include "../components/light.hpp"
#include "../components/renderer.hpp"

namespace hawk
{
	namespace Managers
	{
		/**
		 * Graphics manager.
	     */
		class Video : public Manager
		{
		public:
			/**
			 * Rendering enum.
			 */
			enum class Rendering
			{
				Forward = 1,
				Deferred
			};

			/**
			 * Lighting enum.
			 */
			enum class Lighting
			{
				BlinnPhong = 1,
				PBR
			};

			/**
			 * Light struct.
			 */
			struct Light
			{
				int id;
				int type;
				int cast_shadows;
				unsigned int shadow_map_id;
				glm::vec4 color;
				glm::vec4 position;
				glm::vec4 direction;
				float range;
				float cut_off;
				float outer_cut_off;
				float far_plane;
				glm::mat4 matrix;
			};

			/**
			 * Constructor.
			 */
			Video();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Video(const Video& other) = delete;

			/**
			 * Destructor.
			 */
			~Video();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Video& other) = delete;

			/**
			 * Initializer.
			 * @param width Resolution width.
			 * @param height Resolution height.
			 * @param rendering Rendering path.
			 * @param lighting Lighting method.
			 * @param max_lights Total lights count.
			 * @param max_shadows Total shadows count.
			 * @return True or False.
			 */
			bool initialize(int width, int height, Rendering rendering = Rendering::Forward, Lighting lighting = Lighting::BlinnPhong, unsigned int max_lights = 4, unsigned int max_shadows = 4);

			/**
			 * Deinitializer.
			 */
			void deinitialize();

			/**
			 * Release video.
			 */
			void release();

			/**
			 * Reset graphics states.
			 */
			void reset();

			/**
			 * Start.
			 */
			void start();

			/**
			 * Render. (Dual Depth Peeling)
			 */
			void render();

			/**
			 * Draw main camera frame buffer to back buffer.
			 */
			void postProcess();

			/**
			 * Clear bound buffers.
			 * @param color Clear color.
			 * @param bits Buffer bits.
			 */
			void clear(const glm::vec4& color = glm::vec4(0.0f), unsigned int bits = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

			/**
			 * Draw a mesh.
			 * @param mesh Mesh to draw.
			 * @param instances Count of instances to draw.
			 */
			void draw(const hawk::Assets::Mesh* mesh, int instances = 0);

			/**
			 * Rendering path.
			 */
			Rendering getRendering() const;

			/**
			 * Lighting method.
			 */
			Lighting getLighting() const;

			/**
			 * Max allowed lights.
			 */
			unsigned int getMaxLights() const;

			/**
			 * Max allowed shadows.
			 */
			unsigned int getMaxShadows() const;

			/**
			 * Video mode.
			 */
			const GLFWvidmode* getVideoMode();

			/**
			 * Shaders macros string.
			 */
			const std::string& getShadersMacros() const;

			/**
			 * Current render camera.
			 */
			hawk::Components::Camera* getRenderCamera() const;

			/**
			 * Write or ignore color values.
			 */
			void setColorMask(bool mask);

			/**
			 * Set depth test.
			 */
			void setDepthTest(bool test);

			/**
			 * Set depth test function.
			 */
			void setDepthFunction(unsigned int function);

			/**
			 * Write or ignore depth values.
			 */
			void setDepthMask(bool mask);

			/**
			 * Set stencil test.
			 */
			void setStencilTest(bool test);

			/**
			 * Set stencil test function.
			 */
			void setStencilFunction(unsigned int function, int reference, unsigned int mask);

			/**
			 * Set stencil test operation.
			 */
			void setStencilOperation(unsigned int fail, unsigned int zfail, unsigned int zpass);

			/**
			 * Write or ignore stencil values.
			 */
			void setStencilMask(unsigned int mask);

			/**
			 * Set color blend.
			 */
			void setBlend(bool blend);

			/**
			 * Set blend function.
			 */
			void setBlendFunction(unsigned int src, unsigned int dst);

			/**
			 * Set blend equation.
			 */
			void setBlendEquation(unsigned int equation);

			/**
			 * Set blend constant color.
			 */
			void setBlendColor(const glm::vec4& color);

			/**
			 * Set face cull.
			 */
			void setFaceCull(bool cull);

			/**
			 * Set face cull side.
			 */
			void setFaceSide(unsigned int side);

			/**
			 * Set face cull wind direction.
			 */
			void setFaceOrientation(unsigned int orientation);

			/**
			 * Main camera.
			 */
			hawk::Components::Camera* camera;

			/**
			 * Ambient color.
			 */
			glm::vec4 ambient;

			/**
			 * Screen space ambient occlusion.
			 */
			bool ssao;

			/**
			 * Fast approxmiation anti anliasing.
			 */
			bool fxaa;

			/**
			 * Bloom effect.
			 */
			bool bloom;

			/**
			 * Scene cameras.
			 */
			std::vector<hawk::Components::Camera*> cameras;

			/**
			 * Light casters.
			 */
			std::vector<hawk::Components::Light*> lights;

			/**
			 * Object renderers.
			 */
			std::vector<hawk::Components::Renderer*> renderers;

		private:
			/**
			 * Forward render.
			 */
			void renderForward();

			/**
			 * Deferred render.
			 */
			void renderDeferred();

			/**
			 * Render starting screen quad.
			 * @param camera Camera to be rendered.
			 */
			void renderScreen(const Components::Camera* camera);

			/**
			 * Render final screen quad with gamma correction and hdr.
			 * @param camera Camera to be rendered.
			 */
			void renderFinal(const Components::Camera* camera);

			/**
			 * Render screen to backbuffer.
			 * @param camera Camera to be rendered.
			 */
			void renderBackbuffer(const Components::Camera* camera);

			/**
			 * Apply screen space ambient occlusion.
			 * @param camera Camera to be rendered.
			 */
			void applySSAO(const Components::Camera* camera);

			/**
			 * Apply fast approximation anti aliasing.
			 * @param camera Camera to be rendered.
			 */
			void applyFXAA(const Components::Camera* camera);

			/**
			 * Apply bloom effect.
			 * @param camera Camera to be rendered.
			 */
			void applyBloom(const Components::Camera* camera);

			/**
			 * Rendering path.
			 */
			Rendering m_rendering;

			/**
			 * Lighting method.
			 */
			Lighting m_lighting;

			/**
			 * Max lights count.
			 */
			unsigned int m_max_lights;

			/**
			 * Max shadows count.
			 */
			unsigned int m_max_shadows;

			/**
			 * Video mode.
			 */
			const GLFWvidmode* m_video_mode;

			/**
			 * Shaders macros string.
			 */
			std::string m_shaders_macros;

			/**
			 * Current render camera.
			 */
			hawk::Components::Camera* m_render_camera;

			/**
			 * Color mask cache.
			 */
			bool m_color_mask;

			/**
			 * Depth test cache.
			 */
			bool m_depth_test;

			/**
			 * Depth test function cache.
			 */
			unsigned int m_depth_func;

			/**
			 * Depth test mask cache.
			 */
			bool m_depth_mask;

			/**
			 * Stencil test cache.
			 */
			bool m_stencil_test;

			/**
			 * Stencil test function cache.
			 */
			unsigned int m_stencil_func;

			/**
			 * Stencil test function reference value cache.
			 */
			int m_stencil_func_ref;

			/**
			 * Stencil test function mask value cache.
			 */
			unsigned int m_stencil_func_mask;

			/**
			 * Stencil test operation fail value cache.
			 */
			unsigned int m_stencil_op_fail;

			/**
			 * Stencil test operation zfail value cache.
			 */
			unsigned int m_stencil_op_zfail;

			/**
			 * Stencil test operation zpass value cache.
			 */
			unsigned int m_stencil_op_zpass;

			/**
			 * Stencil test mask cache.
			 */
			unsigned int m_stencil_mask;

			/**
			 * Color blend cache.
			 */
			bool m_blend;

			/**
			 * Blending function source factor cache.
			 */
			unsigned int m_blend_src_factor;

			/**
			 * Blending function destination factor cache.
			 */
			unsigned int m_blend_dst_factor;

			/**
			 * Blending equation cache.
			 */
			unsigned int m_blend_equation;

			/**
			 * Blending constant color cache.
			 */
			glm::vec4 m_blend_color;

			/**
			 * Face cull availability cache.
			 */
			bool m_face_cull;

			/**
			 * Face cull side cache.
			 */
			unsigned int m_face_side;

			/**
			 * Face cull wind direction cache.
			 */
			unsigned int m_face_orientation;

			/**
			 * Video uniform buffer object.
			 */
			unsigned int m_video_ubo;

			/**
			 * Camera uniform buffer object.
			 */
			unsigned int m_camera_ubo;

			/**
			 * Scene uniform buffer object.
			 */
			unsigned int m_scene_ubo;

			/**
			 * Main frame buffer.
			 */
			hawk::Assets::Framebuffer* m_main_framebuffer;

			/**
			 * SSAO frame buffers.
			 */
			hawk::Assets::Framebuffer* m_ssao_framebuffers[2];

			/**
			 * Blur frame buffers.
			 */
			hawk::Assets::Framebuffer* m_blur_framebuffers[2];

			/**
			 * SSAO sample kernel.
			 */
			std::vector<glm::vec3> m_ssao_kernel;

			/**
			 * SSAO noise texture.
			 */
			hawk::Assets::Texture* m_ssao_noise_texture;

			/**
			 * Last operated blur frame buffer.
			 */
			unsigned int m_last_blur_framebuffer;

			/**
			 * Post-process screen mesh.
			 */
			hawk::Assets::Mesh* m_screen_mesh;

			/**
			 * Skybox mesh.
			 */
			hawk::Assets::Mesh* m_skybox_mesh;

			/**
			 * Directional shadow mapping material.
			 */
			hawk::Assets::Material* m_dsm_material;

			/**
			 * Omnidirectional shadow mapping material.
			 */
			hawk::Assets::Material* m_odsm_material;

			/**
			 * Deferred lighting material.
			 */
			hawk::Assets::Material* m_deferred_material;

			/**
			 * Screen quad post process material.
			 */
			hawk::Assets::Material* m_screen_material;

			/**
			 * SSAO material.
			 */
			hawk::Assets::Material* m_ssao_material;

			/**
			 * SSAO blur material.
			 */
			hawk::Assets::Material* m_ssao_blur_material;

			/**
			 * FXAA material.
			 */
			hawk::Assets::Material* m_fxaa_material;

			/**
			 * Blur material.
			 */
			hawk::Assets::Material* m_blur_material;

			/**
			 * Gamma correction and hdr material.
			 */
			hawk::Assets::Material* m_final_material;
		};
	}
}
#endif