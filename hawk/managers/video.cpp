#include "../system/core.hpp"
#include "video.hpp"
#include "../objects/entity.hpp"
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include "../helpers/memory.hpp"
#include <random>

namespace hawk
{
	namespace Managers
	{
		Video::Video() : camera(nullptr), ambient(0.2f, 0.2f, 0.2f, 1.0f), ssao(false), fxaa(false), bloom(false),
			m_rendering(Rendering::Forward), m_lighting(Lighting::BlinnPhong), m_max_lights(4), m_max_shadows(4), m_video_mode(nullptr),
			m_color_mask(false), m_depth_test(false), m_depth_func(GL_NONE), m_depth_mask(false), m_stencil_test(false), m_stencil_func(GL_NONE), m_stencil_func_ref(0), m_stencil_func_mask(0x00), 
			m_stencil_op_fail(GL_NONE), m_stencil_op_zfail(GL_NONE), m_stencil_op_zpass(GL_NONE), m_stencil_mask(0x00), 
			m_blend(false), m_blend_src_factor(GL_NONE), m_blend_dst_factor(GL_NONE), m_blend_equation(GL_NONE), m_blend_color(0.0f), 
			m_face_cull(false), m_face_side(GL_NONE), m_face_orientation(GL_NONE), m_render_camera(nullptr), m_video_ubo(0), m_camera_ubo(0), m_scene_ubo(0), 
			m_main_framebuffer(nullptr), m_ssao_noise_texture(nullptr), m_last_blur_framebuffer(0),
			m_screen_mesh(nullptr), m_dsm_material(nullptr), m_odsm_material(nullptr), m_deferred_material(nullptr), m_screen_material(nullptr),
			m_ssao_material(nullptr), m_ssao_blur_material(nullptr), m_fxaa_material(nullptr), m_blur_material(nullptr), m_final_material(nullptr)
		{
			// set manager name
			m_name = "Video";

			// zeroize
			hawk::Helpers::Memory::zeroize(m_ssao_framebuffers, sizeof m_ssao_framebuffers);
			hawk::Helpers::Memory::zeroize(m_blur_framebuffers, sizeof m_blur_framebuffers);
		}

		Video::~Video() {}

		bool Video::initialize(int width, int height, Rendering rendering, Lighting lighting, unsigned int max_lights, unsigned int max_shadows)
		{
			// application
			System::Core& app = System::Core::getInstance();

			// get video mode
			m_video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

			// reset graphics to initial states
			reset();

			// enable interpolation between cube map faces (prevent uneven faces artifact for mipped radiance maps)
			glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

			// make video ubo
			glGenBuffers(1, &m_video_ubo);
			glBindBuffer(GL_UNIFORM_BUFFER, m_video_ubo);
			glBufferData(GL_UNIFORM_BUFFER, 24, nullptr, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_video_ubo);

			// make camera ubo
			glGenBuffers(1, &m_camera_ubo);
			glBindBuffer(GL_UNIFORM_BUFFER, m_camera_ubo);
			glBufferData(GL_UNIFORM_BUFFER, 164, nullptr, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_camera_ubo);

			// make scene ubo
			glGenBuffers(1, &m_scene_ubo);
			glBindBuffer(GL_UNIFORM_BUFFER, m_scene_ubo);
			glBufferData(GL_UNIFORM_BUFFER, m_max_lights * sizeof Light, nullptr, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_scene_ubo);

			// make main frame buffer
			m_main_framebuffer = new hawk::Assets::Framebuffer();

			if (!m_main_framebuffer->build(width, height, GL_TEXTURE_2D,
				GL_CLAMP_TO_EDGE, GL_LINEAR, glm::vec4(1.0f),
				hawk::Assets::Framebuffer::Attachment::Color | hawk::Assets::Framebuffer::Attachment::Depth | hawk::Assets::Framebuffer::Attachment::Stencil,
				std::vector<hawk::Assets::Framebuffer::BufferFormat> { { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT }, { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT } }))
			{
				app.logger.error("Failed to create main frame buffer!");

				// release frame buffer
				m_main_framebuffer->release();

				m_main_framebuffer = nullptr;

				return false;
			}

			// make ssao frame buffers
			for (int i = 0; i < 2; ++i)
			{
				m_ssao_framebuffers[i] = new hawk::Assets::Framebuffer();

				if (!m_ssao_framebuffers[i]->build(width, height, GL_TEXTURE_2D,
					GL_CLAMP_TO_EDGE, GL_NEAREST, glm::vec4(1.0f),
					hawk::Assets::Framebuffer::Attachment::Color,
					std::vector<hawk::Assets::Framebuffer::BufferFormat> { { GL_RED, GL_RGB, GL_FLOAT } }))
				{
					app.logger.error("Failed to create ssao frame buffers!");

					// release frame buffer
					if (m_ssao_framebuffers[0])
					{
						m_ssao_framebuffers[0]->release();

						m_ssao_framebuffers[0] = nullptr;
					}

					// release frame buffer
					if (m_ssao_framebuffers[1])
					{
						m_ssao_framebuffers[1]->release();

						m_ssao_framebuffers[1] = nullptr;
					}

					return false;
				}
			}

			// make blur frame buffers
			for (int i = 0; i < 2; ++i)
			{
				m_blur_framebuffers[i] = new hawk::Assets::Framebuffer();

				if (!m_blur_framebuffers[i]->build(width, height, GL_TEXTURE_2D,
					GL_CLAMP_TO_EDGE, GL_LINEAR, glm::vec4(1.0f),
					hawk::Assets::Framebuffer::Attachment::Color,
					std::vector<hawk::Assets::Framebuffer::BufferFormat> { { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT } }))
				{
					app.logger.error("Failed to create blur frame buffers!");

					// release frame buffer
					if (m_blur_framebuffers[0])
					{
						m_blur_framebuffers[0]->release();

						m_blur_framebuffers[0] = nullptr;
					}

					// release frame buffer
					if (m_blur_framebuffers[1])
					{
						m_blur_framebuffers[1]->release();

						m_blur_framebuffers[1] = nullptr;
					}

					return false;
				}
			}

			// if deferred rendering
			if (rendering == Rendering::Deferred)
			{
				// default random engine
				std::default_random_engine rand_engine;

				// ssao sample random distribution
				std::uniform_real_distribution<float> random_floats(0.0f, 1.0f);

				// generate ssao sample kernel
				for (unsigned int i = 0; i < 64; ++i)
				{
					// hemisphere sample in tangent space
					glm::vec3 sample(random_floats(rand_engine) * 2.0f - 1.0f, random_floats(rand_engine) * 2.0f - 1.0f, random_floats(rand_engine));

					// normalize sample
					sample = glm::normalize(sample);

					// randomize sample
					sample *= random_floats(rand_engine);

					// sample scale
					float scale = (float)i / 64.0f;

					// lerp scale towards kernel center
					scale = 0.1f + (scale * scale) * (1.0f - 0.1f);

					// align sample to kernel center
					sample *= scale;

					// push to the list
					m_ssao_kernel.push_back(sample);
				}

				// ssao noise vectors
				std::vector<glm::vec3> ssao_noise;

				// generate ssao noise
				for (unsigned int i = 0; i < 16; ++i)
				{
					// noise vector in tangent space rotating around z axis
					glm::vec3 noise(random_floats(rand_engine) * 2.0f - 1.0f, random_floats(rand_engine) * 2.0f - 1.0f, 0.0f);

					// push to the list
					ssao_noise.push_back(noise);
				}

				// make ssao noise texture
				m_ssao_noise_texture = new hawk::Assets::Texture();

				// build texture
				if (!m_ssao_noise_texture->build(std::vector<int> { 4 }, std::vector<int> { 4 }, std::vector<const void*> { &ssao_noise[0] }, -1, GL_TEXTURE_2D, GL_RGB32F, GL_RGB, GL_FLOAT, GL_REPEAT, GL_NEAREST, GL_NEAREST))
				{
					app.logger.error("Failed to create SSAO noise texture!");

					return false;
				}
			}

			// shaders macros
			std::stringstream macros;

			// add shaders macros
			macros << "#define MAX_LIGHTS " << max_lights << std::endl;
			macros << "#define MAX_SHADOWS " << max_shadows << std::endl;
			macros << "#define MAX_REFLECTION_LOD 4.0";

			// set shaders macros
			m_shaders_macros = macros.str();

			// set properties
			m_rendering = rendering;
			m_lighting = lighting;
			m_max_lights = max_lights;
			m_max_shadows = max_shadows;

			return true;
		}

		void Video::deinitialize()
		{
			// free world uniform buffer
			if (m_video_ubo)
			{
				glDeleteBuffers(1, &m_video_ubo);

				m_video_ubo = 0;
			}

			// free camera uniform buffer
			if (m_camera_ubo)
			{
				glDeleteBuffers(1, &m_camera_ubo);

				m_camera_ubo = 0;
			}

			// free main frame buffer
			if (m_main_framebuffer)
			{
				m_main_framebuffer->release();

				m_main_framebuffer = nullptr;
			}

			// free ssao frame buffers
			for (int i = 0; i < 2; ++i)
			{
				if (m_ssao_framebuffers[i])
				{
					m_ssao_framebuffers[i]->release();

					m_ssao_framebuffers[i] = nullptr;
				}
			}

			// free blur frame buffers
			for (int i = 0; i < 2; ++i)
			{
				if (m_blur_framebuffers[i])
				{
					m_blur_framebuffers[i]->release();

					m_blur_framebuffers[i] = nullptr;
				}
			}

			// free ssao noise texture
			if (m_ssao_noise_texture)
			{
				m_ssao_noise_texture->release();

				m_ssao_noise_texture = nullptr;
			}

			// free resources
			release();
		}

		void Video::release()
		{
			// reset variables
			camera = nullptr;
			ambient = glm::vec4(0.2f);
			m_render_camera = nullptr;

			// clear lists
			cameras.clear();
			lights.clear();
			renderers.clear();
		}

		void Video::reset()
		{
			// configure color buffer
			setColorMask(true);

			// configure depth test
			setDepthTest(true);
			setDepthFunction(GL_LESS);
			setDepthMask(GL_TRUE);

			// configure stencil test
			setStencilTest(true);
			setStencilFunction(GL_ALWAYS, 1, 0xFF);
			setStencilOperation(GL_KEEP, GL_KEEP, GL_REPLACE);
			setStencilMask(0xFF);

			// configure blending
			setBlend(true);
			setBlendFunction(GL_ONE, GL_ZERO);
			setBlendEquation(GL_FUNC_ADD);
			setBlendColor(glm::vec4(1.0f));

			// configure face culling
			setFaceCull(true);
			setFaceSide(GL_BACK);
			setFaceOrientation(GL_CCW);

			// configure fill mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		
		void Video::start()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// get screen mesh
			m_screen_mesh = app.assets.meshes["Screen"];
			m_skybox_mesh = app.assets.meshes["Skybox"];

			// get internal materials
			m_dsm_material = app.assets.materials["DirectionalShadowMap"];
			m_odsm_material = app.assets.materials["OmnidirectionalShadowMap"];
			m_screen_material = app.assets.materials["PostProcessScreen"];
			m_ssao_material = app.assets.materials["PostProcessSSAO"];
			m_ssao_blur_material = app.assets.materials["PostProcessSSAOBlur"];
			m_fxaa_material = app.assets.materials["PostProcessFXAA"];
			m_blur_material = app.assets.materials["PostProcessBlur"];
			m_final_material = app.assets.materials["PostProcessFinal"];

			// get deferred lighting material
			if (m_rendering == Rendering::Deferred)
				m_deferred_material = app.assets.materials["DeferredLighting"];

			// set ssao sample kernel uniform
			m_ssao_material->getShader()->setFloatVec3("kernel", m_ssao_kernel);
		}

		void Video::render()
		{
			// ssao integer equivalent
			int ssao_int = (ssao) ? 1 : 0;

			// bloom integer equivalent
			int bloom_int = (bloom) ? 1 : 0;

			// fill video ubo
			glBindBuffer(GL_UNIFORM_BUFFER, m_video_ubo);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, 16, glm::value_ptr(ambient));
			glBufferSubData(GL_UNIFORM_BUFFER, 16, 4, &ssao_int);
			glBufferSubData(GL_UNIFORM_BUFFER, 20, 4, &bloom_int);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			// fill scene ubo
			glBindBuffer(GL_UNIFORM_BUFFER, m_scene_ubo);

			// fill scene lights
			if (!lights.empty())
			{
				// list of active lights
				std::vector<Light> scene_lights;

				// shadow map id holder
				unsigned int shadow_map_id = 0;

				// scene light
				Light light;

				// set lights
				for (unsigned int i = 0; i < m_max_lights; ++i)
				{
					// break if light is out of active lights range
					if (i + 1 > lights.size()) break;

					// continue if light is disabled
					if (!lights[i]->enable) continue;

					// set shared properties
					light.id = i;
					light.type = (int)lights[i]->getLightType();
					light.cast_shadows = lights[i]->cast_shadows ? 1 : 0;
					light.shadow_map_id = (lights[i]->cast_shadows && shadow_map_id < m_max_shadows) ? shadow_map_id++ : 0;
					light.color = lights[i]->color * lights[i]->intensity;
					light.direction = glm::vec4(lights[i]->entity->transform->getWorldRotation(), 0.0f);
					light.position = glm::vec4(lights[i]->entity->transform->getWorldPosition(), 0.0f);
					light.range = lights[i]->range;
					light.cut_off = lights[i]->cut_off;
					light.outer_cut_off = lights[i]->outer_cut_off;
					light.far_plane = lights[i]->far_plane;
					light.matrix = (lights[i]->getLightType() != Components::Light::Type::Point) ? lights[i]->getProjectionMatrix() * lights[i]->getViewMatrix() : glm::mat4(1.0f);
				}

				// push to the list
				scene_lights.push_back(light);

				// fill scene lights
				glBufferData(GL_UNIFORM_BUFFER, scene_lights.size() * sizeof Light, &scene_lights[0], GL_DYNAMIC_DRAW);
			}

			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			// check rendering path
			switch (m_rendering)
			{
			case Rendering::Forward:
				// forward rendering
				renderForward();
				break;
			case Rendering::Deferred:
				// deferred rendering
				renderDeferred();
				break;
			}
		}

		void Video::postProcess()
		{
			// disable graphics state
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_STENCIL_TEST);
			glDisable(GL_BLEND);

			// configure cull face
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);

			// iterate cameras
			for (auto camera : cameras)
			{
				// continue if camera is disabled
				if (!camera->enable) continue;

				// render screen quad
				renderScreen(camera);

				// apply fxaa
				if (fxaa) applyFXAA(camera);

				// apply bloom effect
				if (bloom) applyBloom(camera);

				// render final quad
				renderFinal(camera);

				// render main camera to backbuffer
				if (camera == this->camera)
					renderBackbuffer(camera);
			}

			// enable graphics state
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_STENCIL_TEST);
			glEnable(GL_BLEND);
		}

		void Video::clear(const glm::vec4& color, unsigned int bits)
		{
			// set clear color
			glClearColor(color.r, color.g, color.b, color.a);

			// clear bound buffers
			glClear(bits);
		}

		void Video::draw(const hawk::Assets::Mesh* mesh, int instances)
		{
			// bind vertex array
			glBindVertexArray(mesh->getVao());

			// if mesh does not have vertex indices
			if (mesh->indices.size() == 0)
			{
				// draw using vertex buffer
				if (instances == 0)
					glDrawArrays(mesh->mode, 0, (int)mesh->vertices.size());
				else
					glDrawArraysInstanced(mesh->mode, 0, (int)mesh->vertices.size(), instances);
			}
			else
			{
				// draw using index buffer
				if (instances == 0)
					glDrawElements(mesh->mode, (int)mesh->indices.size(), GL_UNSIGNED_INT, 0);
				else
					glDrawElementsInstanced(mesh->mode, (int)mesh->indices.size(), GL_UNSIGNED_INT, 0, instances);
			}

			// unbind vertex array
			glBindVertexArray(0);
		}

		Video::Rendering Video::getRendering() const
		{
			return m_rendering;
		}

		Video::Lighting Video::getLighting() const
		{
			return m_lighting;
		}

		unsigned int Video::getMaxLights() const
		{
			return m_max_lights;
		}

		unsigned int Video::getMaxShadows() const
		{
			return m_max_shadows;
		}

		const GLFWvidmode* Video::getVideoMode()
		{
			return m_video_mode;
		}

		const std::string& Video::getShadersMacros() const
		{
			return m_shaders_macros;
		}

		hawk::Components::Camera* Video::getRenderCamera() const
		{
			return m_render_camera;
		}

		void Video::setColorMask(bool mask)
		{
			// if cached value is old
			if (m_color_mask != mask)
			{
				// set new value
				m_color_mask = mask;

				// set color mask
				glColorMask(m_color_mask, m_color_mask, m_color_mask, m_color_mask);
			}
		}

		void Video::setDepthTest(bool test)
		{
			// if cached value is old
			if (m_depth_test != test)
			{
				// set new value
				m_depth_test = test;

				// set depth test
				if (m_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
			}
		}

		void Video::setDepthFunction(unsigned int function)
		{
			// if cached value is old
			if (m_depth_func != function)
			{
				// set new value
				m_depth_func = function;

				// set depth function
				glDepthFunc(m_depth_func);
			}
		}

		void Video::setDepthMask(bool mask)
		{
			// if cached value is old
			if (m_depth_mask != mask)
			{
				// set new value
				m_depth_mask = mask;

				// set depth mask
				glDepthMask(m_depth_mask);
			}
		}

		void Video::setStencilTest(bool test)
		{
			// if cached value is old
			if (m_stencil_test != test)
			{
				// set new value
				m_stencil_test = test;

				// set stencil test
				if (m_stencil_test) glEnable(GL_STENCIL_TEST); else glDisable(GL_STENCIL_TEST);
			}
		}

		void Video::setStencilFunction(unsigned int function, int reference, unsigned int mask)
		{
			// if cached value is old
			if (m_stencil_func != function || m_stencil_func_ref != reference || m_stencil_func_mask != mask)
			{
				// set new value
				m_stencil_func = function;
				m_stencil_func_ref = reference;
				m_stencil_func_mask = mask;

				// set stencil function
				glStencilFunc(m_stencil_func, m_stencil_func_ref, m_stencil_func_mask);
			}
		}

		void Video::setStencilOperation(unsigned int fail, unsigned int zfail, unsigned int zpass)
		{
			// if cached value is old
			if (m_stencil_op_fail != fail || m_stencil_op_zfail != zfail || m_stencil_op_zpass != zpass)
			{
				// set new value
				m_stencil_op_fail = fail;
				m_stencil_op_zfail = zfail;
				m_stencil_op_zpass = zpass;

				// set stencil operation
				glStencilOp(m_stencil_op_fail, m_stencil_op_zfail, m_stencil_op_zpass);
			}
		}

		void Video::setStencilMask(unsigned int mask)
		{
			// if cached value is old
			if (m_stencil_mask != mask)
			{
				// set new value
				m_stencil_mask = mask;

				// set stencil mask
				glStencilMask(m_stencil_mask);
			}
		}

		void Video::setBlend(bool blend)
		{
			// if cached value is old
			if (m_blend != blend)
			{
				// set new value
				m_blend = blend;

				// set color blend
				if (m_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
			}
		}

		void Video::setBlendFunction(unsigned int src, unsigned int dst)
		{
			// if cached value is old
			if (m_blend_src_factor != src || m_blend_dst_factor != dst)
			{
				// set new value
				m_blend_src_factor = src;
				m_blend_dst_factor = dst;

				// set blend function
				glBlendFunc(m_blend_src_factor, m_blend_dst_factor);
			}
		}

		void Video::setBlendEquation(unsigned int equation)
		{
			// if cached value is old
			if (m_blend_equation != equation)
			{
				// set new value
				m_blend_equation = equation;

				// set blend equation
				glBlendEquation(m_blend_equation);
			}
		}

		void Video::setBlendColor(const glm::vec4& color)
		{
			// if cached value is old
			if (m_blend_color != color)
			{
				// set new value
				m_blend_color = color;

				// set blend constant color
				glBlendColor(m_blend_color.r, m_blend_color.g, m_blend_color.b, m_blend_color.a);
			}
		}

		void Video::setFaceCull(bool cull)
		{
			// if cached value is old
			if (m_face_cull != cull)
			{
				// set new value
				m_face_cull = cull;

				// set face cull
				if (m_face_cull) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
			}
		}

		void Video::setFaceSide(unsigned int side)
		{
			// if cached value is old
			if (m_face_side != side)
			{
				// set new value
				m_face_side = side;

				// set face side
				glCullFace(m_face_side);
			}
		}

		void Video::setFaceOrientation(unsigned int orientation)
		{
			// if cached value is old
			if (m_face_orientation != orientation)
			{
				// set new value
				m_face_orientation = orientation;

				// set face orientation
				glFrontFace(m_face_orientation);
			}
		}

		void Video::renderForward()
		{
			// render lights for shadows
			for (auto light : lights)
			{
				// continue if light is disabled or is not casting shadows
				if (!light->enable || !light->cast_shadows) continue;

				// bind light shadow map frame buffer
				light->getShadowMapFramebuffer()->bind();

				// clear frame buffer depth
				light->getShadowMapFramebuffer()->clear(glm::vec4(0.0f), GL_DEPTH_BUFFER_BIT);

				// cull front faces for peter panning
				setFaceSide(GL_FRONT);

				// render objects
				for (auto i = renderers.begin(); i != renderers.end(); ++i)
				{
					// continue if mesh renderer is disabled or does not cast shadows or transparent
					if (!(*i)->enable || !(*i)->cast_shadows || (*i)->materials[0]->getMaterialType() == hawk::Assets::Material::Type::Transparent) continue;

					// render for light shadow map
					(*i)->render(light->getLightType() != Components::Light::Type::Point ? m_dsm_material : m_odsm_material, light);
				}

				// reset face culling
				setFaceSide(GL_BACK);

				// unbind light shadow map frame buffer
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			// render scene per camera
			for (auto camera : cameras)
			{
				// continue if camera is disabled
				if (!camera->enable) continue;

				// set render camera
				m_render_camera = camera;

				// fill camera ubo
				m_render_camera->fillCameraBuffer(m_camera_ubo);

				// pre-render scene
				m_render_camera->preRender();

				// render skybox
				if (m_render_camera->skybox)
				{
					// set depth to less than equal for some reason about drawing skybox after or before objects for early depth testing that i can't remember :D
					setDepthFunction(GL_LEQUAL);

					// cull skybox front faces since we're inside the skybox
					setFaceSide(GL_FRONT);

					// set shader program
					m_render_camera->skybox->use();

					// set shader samplers
					m_render_camera->skybox->setSamplers();

					// draw skybox
					draw(m_skybox_mesh);

					// reset graphics states
					setDepthFunction(GL_LESS);
					setFaceSide(GL_BACK);
				}

				// transparent objects
				std::vector<Components::Renderer*> transparent_objects;

				// render objects
				for (auto i = renderers.begin(); i != renderers.end(); ++i)
				{
					// continue if mesh renderer is disabled or only draws shadows
					if (!(*i)->enable || (*i)->shadows == Components::Renderer::Shadows::OnlyShadows) continue;

					// render if material is not transparent
					if ((*i)->materials[0]->getMaterialType() != hawk::Assets::Material::Type::Transparent)
					{
						(*i)->render();
					}
					else
					{
						// add to transparent objects list
						transparent_objects.push_back((*i));
					}
				}

				// render transparent objects back-to-front
				for (auto i = transparent_objects.rbegin(); i != transparent_objects.rend(); ++i)
					(*i)->render();

				// post-render scene
				m_render_camera->postRender();
			}
		}

		void Video::renderDeferred()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// render lights for shadows
			for (auto light : lights)
			{
				// continue if light is disabled or is not casting shadows
				if (!light->enable || !light->cast_shadows) continue;

				// bind light shadow map frame buffer
				light->getShadowMapFramebuffer()->bind();

				// clear frame buffer depth
				light->getShadowMapFramebuffer()->clear(glm::vec4(0.0f), GL_DEPTH_BUFFER_BIT);

				// cull front faces
				setFaceSide(GL_FRONT);

				// render objects
				for (auto i = renderers.begin(); i != renderers.end(); ++i)
				{
					// continue if mesh renderer is disabled or does not cast shadows or transparent
					if (!(*i)->enable || !(*i)->cast_shadows || (*i)->materials[0]->getMaterialType() == hawk::Assets::Material::Type::Transparent) continue;

					// render for light shadow map
					(*i)->render(light->getLightType() != Components::Light::Type::Point ? m_dsm_material : m_odsm_material, light);
				}

				// reset face culling
				setFaceSide(GL_BACK);

				// unbind light shadow map frame buffer
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			// render scene per camera
			for (auto camera : cameras)
			{
				// continue if camera is disabled
				if (!camera->enable) continue;

				// set render camera
				m_render_camera = camera;

				// fill camera ubo
				m_render_camera->fillCameraBuffer(m_camera_ubo);

				// disable color blend to prevent g-buffer alteration
				setBlend(false);

				// pre-render scene
				m_render_camera->preRender();

				// unlit objects
				std::vector<Components::Renderer*> unlit_objects;

				// transparent objects
				std::vector<Components::Renderer*> transparent_objects;
				
				// render objects
				for (auto i = renderers.begin(); i != renderers.end(); ++i)
				{
					// continue if mesh renderer is disabled or only draws shadows
					if (!(*i)->enable || (*i)->shadows == Components::Renderer::Shadows::OnlyShadows) continue;

					// render if material is lit
					if ((*i)->materials[0]->getMaterialType() == hawk::Assets::Material::Type::Lit)
					{
						(*i)->render();
					}
					else
					{
						// post-pone other material types for forward rendering
						if ((*i)->materials[0]->getMaterialType() == hawk::Assets::Material::Type::Unlit)
						{
							// add to unlit objects list
							unlit_objects.push_back((*i));
						}
						else if ((*i)->materials[0]->getMaterialType() == hawk::Assets::Material::Type::Transparent)
						{
							// add to transparent objects list
							transparent_objects.push_back((*i));
						}
					}
				}

				// disable graphics state
				setDepthTest(false);
				setStencilTest(false);

				// configure cull face
				setFaceCull(true);
				setFaceSide(GL_BACK);
				setFaceOrientation(GL_CCW);

				// apply ssao
				if (ssao) applySSAO(m_render_camera);

				// bind camera default frame buffer
				m_render_camera->getDefaultFramebuffer()->bind();

				// clear frame buffer
				m_render_camera->getDefaultFramebuffer()->clear(m_render_camera->background);

				// set shader program
				m_deferred_material->use();

				// bind camera deferred position buffer
				m_render_camera->getDeferredFramebuffer()->bindColorTexture(0, 0);

				// bind camera deferred normal buffer
				m_render_camera->getDeferredFramebuffer()->bindColorTexture(1, 1);

				// bind camera deferred diffuse buffer
				m_render_camera->getDeferredFramebuffer()->bindColorTexture(2, 2);

				// bind camera deferred specular buffer
				m_render_camera->getDeferredFramebuffer()->bindColorTexture(3, 3);

				// bind camera deferred extra buffer
				m_render_camera->getDeferredFramebuffer()->bindColorTexture(4, 4);

				// set ssao blurred texture for deferred rendering
				if (ssao) m_ssao_framebuffers[1]->bindColorTexture(0, 5);

				// set environment lighting textures for pbr
				if (m_lighting == Lighting::PBR)
				{
					// set irradiance map
					if (m_render_camera->irradiance) m_render_camera->irradiance->bind(6);

					// set radiance map
					if (m_render_camera->radiance) m_render_camera->radiance->bind(7);

					// bind brdf texture
					app.assets.getBRDFTexture()->bind(8);
				}

				// bind lights shadow maps
				if (!lights.empty())
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
					for (unsigned int i = 0; i < m_max_lights; ++i)
					{
						// continue if light is out of active lights range
						if (i + 1 > lights.size()) break;

						// continue if light is disabled
						if (!lights[i]->enable) continue;

						// if light has shadows
						if (lights[i]->cast_shadows && shadow_map_id < m_max_shadows)
						{
							// directional shadow map sample index
							dsm_sampler_index = shadow_map_counter++;

							// omnidirectional shadow map sample index
							odsm_sampler_index = shadow_map_counter++;

							// increase shadow map id
							shadow_map_id++;

							// bind shadow map texture
							switch (lights[i]->getLightType())
							{
							case Components::Light::Type::Directional:
								lights[i]->getShadowMapFramebuffer()->bindDepthStencilTexture(dsm_sampler_index);
								break;
							case Components::Light::Type::Point:
								lights[i]->getShadowMapFramebuffer()->bindDepthStencilTexture(odsm_sampler_index);
								break;
							case Components::Light::Type::Spot:
								lights[i]->getShadowMapFramebuffer()->bindDepthStencilTexture(dsm_sampler_index);
								break;
							}
						}

						// increase lights count
						lights_count++;
					}

					// set lights count uniform
					m_deferred_material->getShader()->setUInt("lights_count", lights_count);
				}

				// draw screen quad
				draw(m_screen_mesh);

				// enable graphics state
				setDepthTest(true);
				setStencilTest(true);
				setBlend(true);

				// copy camera deferred frame buffer depth buffer to camera default frame buffer depth buffer
				hawk::Assets::Framebuffer::blit(m_render_camera->getDeferredFramebuffer(), m_render_camera->getDefaultFramebuffer(),
					m_render_camera->viewport.x, m_render_camera->viewport.y, m_render_camera->viewport.z, m_render_camera->viewport.w,
					m_render_camera->viewport.x, m_render_camera->viewport.y, m_render_camera->viewport.z, m_render_camera->viewport.w,
					GL_DEPTH_BUFFER_BIT, GL_NEAREST);

				// bind camera default frame buffer
				m_render_camera->getDefaultFramebuffer()->bind();

				// render skybox
				if (m_render_camera->skybox)
				{
					// set depth to less than equal for some reason about drawing skybox after or before objects for early depth testing that i can't remember :D
					setDepthFunction(GL_LEQUAL);

					// cull skybox front faces since we're inside the skybox
					setFaceSide(GL_FRONT);

					// set shader program
					m_render_camera->skybox->use();

					// set shader samplers
					m_render_camera->skybox->setSamplers();

					// draw skybox
					draw(m_skybox_mesh);

					// reset graphics states
					setDepthFunction(GL_LESS);
					setFaceSide(GL_BACK);
				}

				// set rendering to forward
				m_rendering = Rendering::Forward;

				// forward render unlit objects
				for (auto i = unlit_objects.begin(); i != unlit_objects.end(); ++i)
					(*i)->render();

				// forward render transparent objects back-to-front
				for (auto i = transparent_objects.rbegin(); i != transparent_objects.rend(); ++i)
					(*i)->render();

				// set rendering back to deferred
				m_rendering = Rendering::Deferred;

				// post-render scene
				m_render_camera->postRender();
			}
		}

		void Video::renderScreen(const Components::Camera* camera)
		{
			// bind camera screen frame buffer
			camera->getScreenFramebuffer()->bind();

			// set shader program
			m_screen_material->use();

			// bind camera default frame buffer color attachment
			camera->getDefaultFramebuffer()->bindColorTexture();

			// draw screen quad
			draw(m_screen_mesh);
		}

		void Video::renderFinal(const Components::Camera* camera)
		{
			// bind camera final frame buffer
			camera->getFinalFramebuffer()->bind();

			// clear frame buffer
			camera->getFinalFramebuffer()->clear();

			// set shader program
			m_final_material->use();

			// bind main camera screen frame buffer color attachment
			camera->getScreenFramebuffer()->bindColorTexture(0, 0);

			// set bloom texture
			if (bloom) m_blur_framebuffers[m_last_blur_framebuffer]->bindColorTexture(0, 1);

			// draw screen quad
			draw(m_screen_mesh);

			// draw screen for custom materials in camera
			for (auto material : camera->materials)
			{
				// continue if material is disabled
				if (!material->enable) continue;

				// set shader program
				material->use();

				// bind final frame buffer color attachment
				camera->getFinalFramebuffer()->bindColorTexture();

				// draw screen quad
				draw(m_screen_mesh);
			}

			// unbind texture
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void Video::renderBackbuffer(const Components::Camera* camera)
		{
			// application
			System::Core& app = System::Core::getInstance();

			// bind back buffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// set viewport
			glViewport(0, 0, app.width, app.height);

			// clear back buffer
			clear();

			// set shader program
			m_screen_material->use();

			// bind camera final frame buffer color attachment
			camera->getFinalFramebuffer()->bindColorTexture();

			// draw screen quad
			draw(m_screen_mesh);

			// unbind texture
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void Video::applySSAO(const Components::Camera* camera)
		{
			// bind ssao frame buffer
			m_ssao_framebuffers[0]->bind();

			// clear frame buffer
			m_ssao_framebuffers[0]->clear(glm::vec4(0.0f), GL_COLOR_BUFFER_BIT);

			// set ssao shader program
			m_ssao_material->use();

			// bind camera deferred position buffer
			camera->getDeferredFramebuffer()->bindColorTexture(0, 0);

			// bind camera deferred normal buffer
			camera->getDeferredFramebuffer()->bindColorTexture(1, 1);

			// bind noise texture
			m_ssao_noise_texture->bind(2);

			// draw screen quad
			draw(m_screen_mesh);

			// bind ssao blur frame buffer
			m_ssao_framebuffers[1]->bind();

			// clear frame buffer
			m_ssao_framebuffers[1]->clear(glm::vec4(0.0f), GL_COLOR_BUFFER_BIT);

			// set ssao blur shader program
			m_ssao_blur_material->use();

			// bind ssao texture
			m_ssao_framebuffers[0]->bindColorTexture();

			// draw screen quad
			draw(m_screen_mesh);
		}

		void Video::applyFXAA(const Components::Camera* camera)
		{
			// bind camera screen frame buffer
			camera->getScreenFramebuffer()->bind();

			// set shader program
			m_fxaa_material->use();

			// bind camera screen frame buffer color attachment
			camera->getScreenFramebuffer()->bindColorTexture();

			// draw screen quad
			draw(m_screen_mesh);
		}

		void Video::applyBloom(const Components::Camera* camera)
		{
			// blur direction
			bool horizontal = true;

			// first pass
			bool first_process = true;

			// blur amount
			unsigned int amount = 10;

			// set shader program
			m_blur_material->use();

			// apply bloom
			for (unsigned int i = 0; i < amount; ++i)
			{
				// current ping pong frame buffer
				unsigned int current = horizontal ? 0 : 1;

				// other ping pong frame buffer
				unsigned int other = horizontal ? 1 : 0;

				// bind frame buffer
				m_blur_framebuffers[current]->bind();

				// set blur direction uniform
				m_blur_material->getShader()->setBool("horizontal", horizontal);

				// bind bloom color attachment
				if (first_process) camera->getScreenFramebuffer()->bindColorTexture(1, 0); else m_blur_framebuffers[other]->bindColorTexture();

				// draw screen quad
				draw(m_screen_mesh);

				// reverse blur direction
				horizontal = !horizontal;

				// disable first process
				if (first_process) first_process = false;

				// set last operated frame buffer index
				m_last_blur_framebuffer = current;
			}
		}
	}
}