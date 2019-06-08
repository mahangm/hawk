#include "../system/core.hpp"
#include "assets.hpp"
#include <sstream>

namespace hawk
{
	namespace Managers
	{
		Assets::Assets() : m_shaders_id(0), m_textures_id(0), m_materials_id(0), m_meshes_id(0), m_models_id(0), m_sounds_id(0), 
			m_brdf_texture(nullptr), m_zero_metallic_texture(nullptr), m_zero_roughness_texture(nullptr), 
			m_full_metallic_texture(nullptr), m_full_roughness_texture(nullptr), m_full_ambient_texture(nullptr)
		{
			// set manager name
			m_name = "Assets";
		}

		Assets::~Assets() {}

		bool Assets::initialize()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// add built-in shaders
			if (!addShader("DirectionalShadowMap", std::vector<std::string> { "../data/shaders/shadow-mapping/directional.vert", "../data/shaders/shadow-mapping/directional.frag" })) return false;
			if (!addShader("OmnidirectionalShadowMap", std::vector<std::string> { "../data/shaders/shadow-mapping/omnidirectional.vert", "../data/shaders/shadow-mapping/omnidirectional.frag", "../data/shaders/shadow-mapping/omnidirectional.geom" })) return false;
			if (!addShader("PostProcessScreen", std::vector<std::string> { "../data/shaders/post-process/screen.vert", "../data/shaders/post-process/screen.frag" })) return false;
			if (!addShader("PostProcessSSAO", std::vector<std::string> { "../data/shaders/post-process/screen.vert", "../data/shaders/post-process/ssao.frag" })) return false;
			if (!addShader("PostProcessSSAOBlur", std::vector<std::string> { "../data/shaders/post-process/screen.vert", "../data/shaders/post-process/blur_ssao.frag" })) return false;
			if (!addShader("PostProcessFXAA", std::vector<std::string> { "../data/shaders/post-process/screen.vert", "../data/shaders/post-process/fxaa.frag" })) return false;
			if (!addShader("PostProcessBlur", std::vector<std::string> { "../data/shaders/post-process/screen.vert", "../data/shaders/post-process/blur_gaussian.frag" })) return false;
			if (!addShader("PostProcessDepth", std::vector<std::string> { "../data/shaders/post-process/screen.vert", "../data/shaders/post-process/depth.frag" })) return false;
			if (!addShader("PostProcessFinal", std::vector<std::string> { "../data/shaders/post-process/screen.vert", "../data/shaders/post-process/final.frag" })) return false;
			if (!addShader("UnlitColored", std::vector<std::string> { "../data/shaders/unlit/colored.vert", "../data/shaders/unlit/colored.frag" })) return false;
			if (!addShader("UnlitTextured", std::vector<std::string> { "../data/shaders/unlit/textured.vert", "../data/shaders/unlit/textured.frag" })) return false;

			// load lighting shaders based on render path and ligthing method
			if (app.video.getRendering() == Managers::Video::Rendering::Forward)
			{
				switch (app.video.getLighting())
				{
				case Managers::Video::Lighting::BlinnPhong:
					if (!addShader("LitColored", std::vector<std::string> { "../data/shaders/lit/forward/blinn-phong/colored.vert", "../data/shaders/lit/forward/blinn-phong/colored.frag" })) return false;
					if (!addShader("LitTextured", std::vector<std::string> { "../data/shaders/lit/forward/blinn-phong/textured.vert", "../data/shaders/lit/forward/blinn-phong/textured.frag" })) return false;
					if (!addShader("LitTexturedNormal", std::vector<std::string> { "../data/shaders/lit/forward/blinn-phong/textured.vert", "../data/shaders/lit/forward/blinn-phong/textured_normal.frag" })) return false;
					if (!addShader("LitTexturedParallax", std::vector<std::string> { "../data/shaders/lit/forward/blinn-phong/textured.vert", "../data/shaders/lit/forward/blinn-phong/textured_parallax.frag" })) return false;
					break;
				case Managers::Video::Lighting::PBR:
					if (!addShader("LitColored", std::vector<std::string> { "../data/shaders/lit/forward/pbr/colored.vert", "../data/shaders/lit/forward/pbr/colored.frag" })) return false;
					if (!addShader("LitTextured", std::vector<std::string> { "../data/shaders/lit/forward/pbr/textured.vert", "../data/shaders/lit/forward/pbr/textured.frag" })) return false;
					if (!addShader("LitTexturedNormal", std::vector<std::string> { "../data/shaders/lit/forward/pbr/textured.vert", "../data/shaders/lit/forward/pbr/textured_normal.frag" })) return false;
					if (!addShader("LitTexturedParallax", std::vector<std::string> { "../data/shaders/lit/forward/pbr/textured.vert", "../data/shaders/lit/forward/pbr/textured_parallax.frag" })) return false;
					break;
				}

				// lit shader
				hawk::Assets::Shader* lit_colored = shaders["LitColored"],
					*lit_textured = shaders["LitTextured"],
					*lit_textured_normal = shaders["LitTexturedNormal"],
					*lit_textured_parallax = shaders["LitTexturedParallax"];

				// max allowed shadows
				unsigned int max_shadows = app.video.getMaxShadows();

				// shadow map texture counter
				int shadow_map_counter = 16;

				// shadow map name
				std::stringstream shadow_map_name;

				// set lights shadow maps sampler index
				for (unsigned int i = 0; i < max_shadows; ++i)
				{
					// make shadow map name
					shadow_map_name << "shadow_maps[" << i << "].";

					// set dsm sampler index
					lit_colored->setInt(shadow_map_name.str().append("dsm").c_str(), shadow_map_counter);
					lit_textured->setInt(shadow_map_name.str().append("dsm").c_str(), shadow_map_counter);
					lit_textured_normal->setInt(shadow_map_name.str().append("dsm").c_str(), shadow_map_counter);
					lit_textured_parallax->setInt(shadow_map_name.str().append("dsm").c_str(), shadow_map_counter);

					// increase shadow map counter
					shadow_map_counter++;

					// set odsm sampler index
					lit_colored->setInt(shadow_map_name.str().append("odsm").c_str(), shadow_map_counter);
					lit_textured->setInt(shadow_map_name.str().append("odsm").c_str(), shadow_map_counter);
					lit_textured_normal->setInt(shadow_map_name.str().append("odsm").c_str(), shadow_map_counter);
					lit_textured_parallax->setInt(shadow_map_name.str().append("odsm").c_str(), shadow_map_counter);

					// increase shadow map counter
					shadow_map_counter++;

					// clear name
					shadow_map_name.str(std::string());
				}
			}
			else if (app.video.getRendering() == Managers::Video::Rendering::Deferred)
			{
				switch (app.video.getLighting())
				{
				case Managers::Video::Lighting::BlinnPhong:
					if (!addShader("LitColored", std::vector<std::string> { "../data/shaders/lit/deferred/blinn-phong/colored.vert", "../data/shaders/lit/deferred/blinn-phong/colored.frag" })) return false;
					if (!addShader("LitColoredForward", std::vector<std::string> { "../data/shaders/lit/forward/blinn-phong/colored.vert", "../data/shaders/lit/forward/blinn-phong/colored.frag" })) return false;
					if (!addShader("LitTextured", std::vector<std::string> { "../data/shaders/lit/deferred/blinn-phong/textured.vert", "../data/shaders/lit/deferred/blinn-phong/textured.frag" })) return false;
					if (!addShader("LitTexturedNormal", std::vector<std::string> { "../data/shaders/lit/deferred/blinn-phong/textured.vert", "../data/shaders/lit/deferred/blinn-phong/textured_normal.frag" })) return false;
					if (!addShader("LitTexturedParallax", std::vector<std::string> { "../data/shaders/lit/deferred/blinn-phong/textured.vert", "../data/shaders/lit/deferred/blinn-phong/textured_parallax.frag" })) return false;
					if (!addShader("LitTexturedForward", std::vector<std::string> { "../data/shaders/lit/forward/blinn-phong/textured.vert", "../data/shaders/lit/forward/blinn-phong/textured.frag" })) return false;
					if (!addShader("LitTexturedNormalForward", std::vector<std::string> { "../data/shaders/lit/forward/blinn-phong/textured.vert", "../data/shaders/lit/forward/blinn-phong/textured_normal.frag" })) return false;
					if (!addShader("LitTexturedParallaxForward", std::vector<std::string> { "../data/shaders/lit/forward/blinn-phong/textured.vert", "../data/shaders/lit/forward/blinn-phong/textured_parallax.frag" })) return false;
					if (!addShader("DeferredLighting", std::vector<std::string> { "../data/shaders/post-process/screen.vert", "../data/shaders/lit/deferred/blinn-phong/lighting.frag" })) return false;
					break;
				case Managers::Video::Lighting::PBR:
					if (!addShader("LitColored", std::vector<std::string> { "../data/shaders/lit/deferred/pbr/colored.vert", "../data/shaders/lit/deferred/pbr/colored.frag" })) return false;
					if (!addShader("LitColoredForward", std::vector<std::string> { "../data/shaders/lit/forward/pbr/colored.vert", "../data/shaders/lit/forward/pbr/colored.frag" })) return false;
					if (!addShader("LitTextured", std::vector<std::string> { "../data/shaders/lit/deferred/pbr/textured.vert", "../data/shaders/lit/deferred/pbr/textured.frag" })) return false;
					if (!addShader("LitTexturedNormal", std::vector<std::string> { "../data/shaders/lit/deferred/pbr/textured.vert", "../data/shaders/lit/deferred/pbr/textured_normal.frag" })) return false;
					if (!addShader("LitTexturedParallax", std::vector<std::string> { "../data/shaders/lit/deferred/pbr/textured.vert", "../data/shaders/lit/deferred/pbr/textured_parallax.frag" })) return false;
					if (!addShader("LitTexturedForward", std::vector<std::string> { "../data/shaders/lit/forward/pbr/textured.vert", "../data/shaders/lit/forward/pbr/textured.frag" })) return false;
					if (!addShader("LitTexturedNormalForward", std::vector<std::string> { "../data/shaders/lit/forward/pbr/textured.vert", "../data/shaders/lit/forward/pbr/textured_normal.frag" })) return false;
					if (!addShader("LitTexturedParallaxForward", std::vector<std::string> { "../data/shaders/lit/forward/pbr/textured.vert", "../data/shaders/lit/forward/pbr/textured_parallax.frag" })) return false;
					if (!addShader("DeferredLighting", std::vector<std::string> { "../data/shaders/post-process/screen.vert", "../data/shaders/lit/deferred/pbr/lighting.frag" })) return false;
					break;
				}

				// lit shader
				hawk::Assets::Shader* lit_colored_forward = shaders["LitColoredForward"], 
					*lit_textured_forward = shaders["LitTexturedForward"],
					*lit_textured_normal_forward = shaders["LitTexturedNormalForward"],
					*lit_textured_parallax_forward = shaders["LitTexturedParallaxForward"];

				// deferred lighting shader
				hawk::Assets::Shader* deferred_lighting = shaders["DeferredLighting"];

				// shadow map name
				std::stringstream shadow_map_name;

				// shadow map texture counter
				int shadow_map_counter = 16;

				// set lights shadow maps sampler index
				for (unsigned int i = 0; i < app.video.getMaxShadows(); ++i)
				{
					// make shadow map name
					shadow_map_name << "shadow_maps[" << i << "].";

					// set shadow map sampler index
					lit_colored_forward->setInt(shadow_map_name.str().append("dsm").c_str(), shadow_map_counter);
					lit_textured_forward->setInt(shadow_map_name.str().append("dsm").c_str(), shadow_map_counter);
					lit_textured_normal_forward->setInt(shadow_map_name.str().append("dsm").c_str(), shadow_map_counter);
					lit_textured_parallax_forward->setInt(shadow_map_name.str().append("dsm").c_str(), shadow_map_counter);
					deferred_lighting->setInt(shadow_map_name.str().append("dsm").c_str(), shadow_map_counter);

					// increase shadow map counter
					shadow_map_counter++;

					lit_colored_forward->setInt(shadow_map_name.str().append("odsm").c_str(), shadow_map_counter);
					lit_textured_forward->setInt(shadow_map_name.str().append("odsm").c_str(), shadow_map_counter);
					lit_textured_normal_forward->setInt(shadow_map_name.str().append("odsm").c_str(), shadow_map_counter);
					lit_textured_parallax_forward->setInt(shadow_map_name.str().append("odsm").c_str(), shadow_map_counter);
					deferred_lighting->setInt(shadow_map_name.str().append("odsm").c_str(), shadow_map_counter);

					// increase shadow map counter
					shadow_map_counter++;

					// clear name
					shadow_map_name.str(std::string());
				}
			}

			// if lighting pbr
			if (app.video.getLighting() == Video::Lighting::PBR)
			{
				// make texture
				m_brdf_texture = new hawk::Assets::Texture();

				// load texture
				if (!m_brdf_texture->load("../data/textures/brdf_integration_map.dds", false, true, -1, GL_RG16F, GL_RG, GL_HALF_FLOAT, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR))
				{
					app.logger.error("Failed to load BRDF look up texture!");

					return false;
				}

				// make texture
				m_zero_metallic_texture = new hawk::Assets::Texture();

				// load texture
				if (!m_zero_metallic_texture->load("../data/textures/zero_metallic.png", false))
				{
					app.logger.error("Failed to load zero metallic texture!");

					return false;
				}

				// make texture
				m_zero_roughness_texture = new hawk::Assets::Texture();

				// load texture
				if (!m_zero_roughness_texture->load("../data/textures/zero_roughness.png", false))
				{
					app.logger.error("Failed to load zero roughness texture!");

					return false;
				}

				// make texture
				m_full_metallic_texture = new hawk::Assets::Texture();

				// load texture
				if (!m_full_metallic_texture->load("../data/textures/full_metallic.png", false))
				{
					app.logger.error("Failed to load full metallic texture!");

					return false;
				}

				// make texture
				m_full_roughness_texture = new hawk::Assets::Texture();

				// load texture
				if (!m_full_roughness_texture->load("../data/textures/full_roughness.png", false))
				{
					app.logger.error("Failed to load full roughness texture!");

					return false;
				}

				// make texture
				m_full_ambient_texture = new hawk::Assets::Texture();

				// load texture
				if (!m_full_ambient_texture->load("../data/textures/full_ambient.png", false))
				{
					app.logger.error("Failed to load full ambient texture!");

					return false;
				}
			}

			// add built-in materials
			if (!addMaterial("DirectionalShadowMap", shaders["DirectionalShadowMap"])) return false;
			if (!addMaterial("OmnidirectionalShadowMap", shaders["OmnidirectionalShadowMap"])) return false;
			if (!addMaterial("PostProcessScreen", shaders["PostProcessScreen"])) return false;
			if (!addMaterial("PostProcessSSAO", shaders["PostProcessSSAO"])) return false;
			if (!addMaterial("PostProcessSSAOBlur", shaders["PostProcessSSAOBlur"])) return false;
			if (!addMaterial("PostProcessFXAA", shaders["PostProcessFXAA"])) return false;
			if (!addMaterial("PostProcessBlur", shaders["PostProcessBlur"])) return false;
			if (!addMaterial("PostProcessDepth", shaders["PostProcessDepth"])) return false;
			if (!addMaterial("PostProcessFinal", shaders["PostProcessFinal"])) return false;

			// add deferred lighting material
			if (app.video.getRendering() == Video::Rendering::Deferred)
				if (!addMaterial("DeferredLighting", shaders["DeferredLighting"])) return false;

			// add built-in meshes
			if (!addMesh("Screen", hawk::Assets::Mesh::Primitive::Quad)) return false;
			if (!addMesh("Skybox", hawk::Assets::Mesh::Primitive::Cube)) return false;

			return true;
		}

		void Assets::deinitialize()
		{
			// free assets
			release();

			// free texture
			if (m_brdf_texture)
			{
				m_brdf_texture->release();

				m_brdf_texture = nullptr;
			}

			// free texture
			if (m_zero_metallic_texture)
			{
				m_zero_metallic_texture->release();

				m_zero_metallic_texture = nullptr;
			}

			// free texture
			if (m_zero_roughness_texture)
			{
				m_zero_roughness_texture->release();

				m_zero_roughness_texture = nullptr;
			}

			// free texture
			if (m_full_metallic_texture)
			{
				m_full_metallic_texture->release();

				m_full_metallic_texture = nullptr;
			}

			// free texture
			if (m_full_roughness_texture)
			{
				m_full_roughness_texture->release();

				m_full_roughness_texture = nullptr;
			}

			// free texture
			if (m_full_ambient_texture)
			{
				m_full_ambient_texture->release();

				m_full_ambient_texture = nullptr;
			}
		}

		hawk::Assets::Shader* Assets::addShader(const char* name, const std::vector<std::string>& paths)
		{
			// shader asset
			hawk::Assets::Shader* asset = new hawk::Assets::Shader();
			asset->m_name = name;

			// load shader sources
			if (!asset->load(paths))
			{
				// delete asset if failed
				delete asset;

				return nullptr;
			}

			// set asset id
			asset->m_id = m_shaders_id++;

			// push to the list
			shaders.insert(shaders.end(), std::make_pair(name, asset));

			return asset;
		}

		void Assets::addShaders(const std::map<std::string, hawk::Assets::Shader*>& shaders)
		{
			// push to the list
			this->shaders.insert(shaders.begin(), shaders.end());

			// increase shaders id
			m_shaders_id += (unsigned int)shaders.size();
		}

		void Assets::removeShader(const char* name)
		{
			// if asset exists
			if (shaders.count(name))
			{
				// release shader
				shaders[name]->release();

				// remove from list
				shaders.erase(name);
			}
		}

		hawk::Assets::Texture* Assets::addTexture(const char* name, const std::string& path,
			bool flip, bool hdr, int mips,
			int internal_format, unsigned int data_format, unsigned int data_type,
			int wrap, int min_filter, int mag_filter)
		{
			// texture asset
			hawk::Assets::Texture* asset = new hawk::Assets::Texture();
			asset->m_name = name;

			// load texture image
			if (!asset->load(path, flip, hdr, mips, internal_format, data_format, data_type, wrap, min_filter, mag_filter))
			{
				// delete asset if failed
				delete asset;

				return nullptr;
			}

			// set asset id
			asset->m_id = m_textures_id++;

			// push to the list
			textures.insert(textures.end(), std::make_pair(name, asset));

			return asset;
		}

		hawk::Assets::Texture* Assets::addTexture(const char* name, const std::vector<std::string>& paths,
			bool flip, bool hdr, int mips,
			int internal_format, unsigned int data_format, unsigned int data_type,
			int wrap, int min_filter, int mag_filter)
		{
			// texture asset
			hawk::Assets::Texture* asset = new hawk::Assets::Texture();
			asset->m_name = name;

			// load textures image
			if (!asset->load(paths, flip, hdr, mips, internal_format, data_format, data_type, wrap, min_filter, mag_filter))
			{
				// delete asset if failed
				delete asset;

				return nullptr;
			}

			// set asset id
			asset->m_id = m_textures_id++;

			// push to the list
			textures.insert(textures.end(), std::make_pair(name, asset));

			return asset;
		}

		void Assets::addTextures(const std::map<std::string, hawk::Assets::Texture*>& textures)
		{
			// push to the list
			this->textures.insert(textures.begin(), textures.end());

			// increase textures id
			m_textures_id += (unsigned int)textures.size();
		}

		void Assets::removeTexture(const char* name)
		{
			// if asset exists
			if (textures.count(name))
			{
				// release texture
				textures[name]->release();

				// remove from list
				textures.erase(name);
			}
		}

		hawk::Assets::Material* Assets::addMaterial(const char* name, hawk::Assets::Shader* shader, hawk::Assets::Material::Type type)
		{
			// material asset
			hawk::Assets::Material* asset = new hawk::Assets::Material(shader, type);
			asset->m_name = name;

			// set asset id
			asset->m_id = m_materials_id++;

			// push to the list
			materials.insert(materials.end(), std::make_pair(name, asset));

			return asset;
		}

		void Assets::addMaterials(const std::map<std::string, hawk::Assets::Material*>& materials)
		{
			// push to the list
			this->materials.insert(materials.begin(), materials.end());

			// increase materials id
			m_materials_id += (unsigned int)materials.size();
		}

		void Assets::removeMaterial(const char* name)
		{
			// if asset exists
			if (materials.count(name))
			{
				// release material
				materials[name]->release();

				// remove from list
				materials.erase(name);
			}
		}

		hawk::Assets::Mesh* Assets::addMesh(const char* name, hawk::Assets::Mesh::Primitive primitive, float width, float height, float length)
		{
			// Mesh asset
			hawk::Assets::Mesh* asset = new hawk::Assets::Mesh(primitive, width, height, length);
			asset->m_name = name;

			// build mesh
			if (!asset->build())
			{
				// delete asset if failed
				delete asset;

				return nullptr;
			}

			// set asset id
			asset->m_id = m_meshes_id++;

			// push to the list
			meshes.insert(meshes.end(), std::make_pair(name, asset));

			return asset;
		}

		void Assets::addMeshes(const std::map<std::string, hawk::Assets::Mesh*>& meshes)
		{
			// push to the list
			this->meshes.insert(meshes.begin(), meshes.end());

			// increase meshes id
			m_meshes_id += (unsigned int)meshes.size();
		}

		void Assets::removeMesh(const char* name)
		{
			// if asset exists
			if (meshes.count(name))
			{
				// release mesh
				meshes[name]->release();

				// remove from list
				meshes.erase(name);
			}
		}

		hawk::Assets::Model* Assets::addModel(const char* name, const std::string& path)
		{
			// model asset
			hawk::Assets::Model* asset = new hawk::Assets::Model();
			asset->m_name = name;

			// load model file
			if (!asset->load(path))
			{
				// delete asset if failed
				delete asset;

				return nullptr;
			}

			// set asset id
			asset->m_id = m_models_id++;

			// push to the list
			models.insert(models.end(), std::make_pair(name, asset));

			return asset;
		}

		void Assets::addModels(const std::map<std::string, hawk::Assets::Model*>& models)
		{
			// push to the list
			this->models.insert(models.begin(), models.end());

			// increase models id
			m_models_id += (unsigned int)models.size();
		}

		void Assets::removeModel(const char* name)
		{
			// if asset exists
			if (models.count(name))
			{
				// release model
				models[name]->release();

				// remove from list
				models.erase(name);
			}
		}

		hawk::Assets::Sound* Assets::addSound(const char* name, const std::string& path, int mode)
		{
			// sound asset
			hawk::Assets::Sound* asset = new hawk::Assets::Sound();
			asset->m_name = name;

			// load sound file
			if (!asset->load(path, mode))
			{
				// delete asset if failed
				delete asset;

				return nullptr;
			}

			// set asset id
			asset->m_id = m_sounds_id++;

			// push to the list
			sounds.insert(sounds.end(), std::make_pair(name, asset));

			return asset;
		}

		void Assets::addSounds(const std::map<std::string, hawk::Assets::Sound*>& sounds)
		{
			// push to the list
			this->sounds.insert(sounds.begin(), sounds.end());

			// increase sounds id
			m_sounds_id += (unsigned int)sounds.size();
		}

		void Assets::removeSound(const char* name)
		{
			// if asset exists
			if (sounds.count(name))
			{
				// release sound
				sounds[name]->release();

				// remove from list
				sounds.erase(name);
			}
		}

		void Assets::release()
		{
			// free shaders
			for (auto asset : shaders)
				if (asset.second) asset.second->release();

			// free textures
			for (auto asset : textures)
				if (asset.second) asset.second->release();

			// free materials
			for (auto asset : materials)
				if (asset.second) asset.second->release();

			// free meshes
			for (auto asset : meshes)
				if (asset.second) asset.second->release();

			// free models
			for (auto asset : models)
				if (asset.second) asset.second->release();

			// free sounds
			for (auto asset : sounds)
				if (asset.second) asset.second->release();

			// clear lists
			shaders.clear();
			textures.clear();
			materials.clear();
			meshes.clear();
			models.clear();
			sounds.clear();

			// reset ids
			m_shaders_id = 0;
			m_textures_id = 0;
			m_materials_id = 0;
			m_meshes_id = 0;
			m_models_id = 0;
			m_sounds_id = 0;
		}

		hawk::Assets::Texture* Assets::getBRDFTexture() const
		{
			return m_brdf_texture;
		}

		hawk::Assets::Texture* Assets::getZeroMetallicTexture() const
		{
			return m_zero_metallic_texture;
		}

		hawk::Assets::Texture* Assets::getZeroRoughnessTexture() const
		{
			return m_zero_roughness_texture;
		}

		hawk::Assets::Texture* Assets::getFullMetallicTexture() const
		{
			return m_full_metallic_texture;
		}

		hawk::Assets::Texture* Assets::getFullRoughnessTexture() const
		{
			return m_full_roughness_texture;
		}

		hawk::Assets::Texture* Assets::getFullAmbientTexture() const
		{
			return m_full_ambient_texture;
		}
	}
}