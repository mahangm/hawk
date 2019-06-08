#include "../system/core.hpp"
#include "model.hpp"
#include <sstream>

namespace hawk
{
	namespace Assets
	{
		Model::Model() : m_importer(nullptr), m_scene(nullptr), m_entity(nullptr)
		{
			// set asset type
			m_type = "Model";

			// application
			System::Core& app = System::Core::getInstance();

			// get shaders
			m_textured_shader = app.assets.shaders["LitTextured"];
			m_textured_normal_shader = app.assets.shaders["LitTexturedNormal"];
			m_textured_parallax_shader = app.assets.shaders["LitTexturedParallax"];
			m_textured_forward_shader = app.assets.shaders["LitTexturedForward"];
			m_textured_normal_forward_shader = app.assets.shaders["LitTexturedNormalForward"];
			m_textured_parallax_forward_shader = app.assets.shaders["LitTexturedParallaxForward"];
		}

		Model::Model(const Model& other) : Asset(other), m_importer(other.m_importer), m_scene(other.m_scene), 
			m_textures(other.m_textures), m_materials(other.m_materials), m_meshes(other.m_meshes), m_entity(other.m_entity), 
			m_textured_shader(other.m_textured_shader), m_textured_normal_shader(other.m_textured_normal_shader), m_textured_parallax_shader(other.m_textured_parallax_shader),
			m_textured_forward_shader(other.m_textured_forward_shader), m_textured_normal_forward_shader(other.m_textured_normal_forward_shader), m_textured_parallax_forward_shader(other.m_textured_parallax_forward_shader)
		{
			// set asset type
			m_type = "Model";
		}

		Model::~Model() {}

		void Model::operator=(const Model& other)
		{
			m_importer = other.m_importer;
			m_scene = other.m_scene;
			m_textures = other.m_textures;
			m_materials = other.m_materials;
			m_meshes = other.m_meshes;
			m_entity = other.m_entity;
			m_textured_shader = other.m_textured_shader;
			m_textured_normal_shader = other.m_textured_normal_shader;
			m_textured_parallax_shader = other.m_textured_parallax_shader;
			m_textured_forward_shader = other.m_textured_forward_shader;
			m_textured_normal_forward_shader = other.m_textured_normal_forward_shader;
			m_textured_parallax_forward_shader = other.m_textured_parallax_forward_shader;
		}

		bool Model::load(const std::string& path)
		{
			// application
			System::Core& app = System::Core::getInstance();

			// set asset path
			setPath(path);

			// make importer
			m_importer = new Assimp::Importer();

			// read model file
			m_scene = m_importer->ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

			// check if model is loaded
			if (!m_scene || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_scene->mRootNode)
			{
				app.logger.error("Failed to load model!");
				app.logger.info(m_importer->GetErrorString());

				return false;
			}

			// make root entity
			m_entity = new Objects::Entity();

			// add root entity components
			m_entity->addComponent("Transform", new Components::Transform());

			// read assimp nodes
			if (!readAssimpNode(m_scene->mRootNode, m_scene, m_entity))
			{
				// free assimp scene
				m_importer->FreeScene();

				return false;
			}

			// free assimp scene
			m_importer->FreeScene();

			return true;
		}

		void Model::release()
		{
			// free importer, frees scene upon destruction
			if (m_importer) delete m_importer;

			// release entity
			if (m_entity)
			{
				m_entity->release();

				m_entity = nullptr;
			}

			// release textures
			for (auto asset : m_textures)
				if (asset) asset->release();

			// release materials
			for (auto asset : m_materials)
				if (asset) asset->release();

			// release meshes
			for (auto asset : m_meshes)
				if (asset) asset->release();

			// clear lists
			m_textures.clear();
			m_materials.clear();
			m_meshes.clear();

			// free object
			delete this;
		}

		Objects::Entity* Model::getEntity() const
		{
			return m_entity;
		}

		bool Model::readAssimpNode(aiNode* node, const aiScene* scene, Objects::Entity* root_entity)
		{
			// hawk entity
			Objects::Entity* hawk_entity = nullptr;

			// read node meshes
			for (unsigned int i = 0; i < node->mNumMeshes; ++i)
			{
				// node mesh
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

				// internal mesh
				Mesh* internal_mesh = nullptr;

				// get mesh
				if (!readAssimpMesh(mesh, scene, internal_mesh))
				{
					// free resources
					releaseLoadingAssets();

					return false;
				}

				// add to model meshes
				m_meshes.insert(m_meshes.end(), internal_mesh);

				// internal material
				Material* internal_material = nullptr;

				// get material
				if (!readAssimpMaterial(scene->mMaterials[mesh->mMaterialIndex], scene, internal_material))
				{
					// free resources
					releaseLoadingAssets();

					return false;
				}

				// add to model materials
				m_materials.insert(m_materials.end(), internal_material);

				// set this node as a single entity if node has no childeren
				if (node->mParent == nullptr && node->mNumChildren == 0)
				{
					// add root entity components
					root_entity->addComponent("MeshRenderer", new Components::MeshRenderer(internal_material, internal_mesh));

					return true;
				}
				else
				{
					// make hawk entity
					hawk_entity = new Objects::Entity();

					// add hawk entity components
					hawk_entity->addComponent("Transform", new Components::Transform());
					hawk_entity->addComponent("MeshRenderer", new Components::MeshRenderer(internal_material, internal_mesh));

					root_entity->addEntity(node->mName.C_Str(), hawk_entity);
				}
			}

			// read child meshes
			for (unsigned int i = 0; i < node->mNumChildren; ++i)
			{
				// read node mesh
				if (!readAssimpNode(node->mChildren[i], scene, (hawk_entity) ? hawk_entity : root_entity))
					return false;
			}

			return true;
		}

		bool Model::readAssimpMesh(aiMesh* mesh, const aiScene* scene, Mesh*& out)
		{
			// vertices
			std::vector<Mesh::Vertex> vertices;

			// get vertices
			for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
			{
				// vertex
				Mesh::Vertex vertex;
				
				// get position
				vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

				// get normal
				vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
				
				// get first uv coords, each vertex can have up to 8 uv coords
				if (mesh->mTextureCoords[0]) vertex.uv = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y); else vertex.uv = glm::vec2(0.0f);

				// get tangent
				if (mesh->mTangents) vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

				// get bitangent
				if (mesh->mBitangents) vertex.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

				// push to the list
				vertices.push_back(vertex);
			}

			// indices
			std::vector<unsigned int> indices;

			// get faces
			for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
			{
				// face
				aiFace face = mesh->mFaces[i];

				// get indices
				for (unsigned int j = 0; j < face.mNumIndices; ++j)
					indices.push_back(face.mIndices[j]);
			}

			// make internal mesh
			out = new Mesh(vertices, indices);

			// build mesh
			if (!out->build())
			{
				// free mesh
				out->release();

				out = nullptr;

				return false;
			}

			return true;
		}

		bool Model::readAssimpMaterial(aiMaterial* material, const aiScene* scene, Material*& out)
		{
			// application
			System::Core& app = System::Core::getInstance();

			// material textures list
			std::vector<std::pair<std::string, Texture*>> material_textures;

			// material transparent flag
			bool material_transparent = false;

			// texture flag
			bool has_normal = false, has_displacement = false;

			// get diffuse textures
			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
			{
				if (!readAssimpTexture(material, aiTextureType_DIFFUSE, material_textures, material_transparent))
				{
					// free textures
					for (auto& material_textures : material_textures)
						material_textures.second->release();

					return false;
				}
			}

			// get specular textures
			if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
			{
				if (!readAssimpTexture(material, aiTextureType_SPECULAR, material_textures, material_transparent))
				{
					// free textures
					for (auto& material_textures : material_textures)
						material_textures.second->release();

					return false;
				}
			}

			// get shininess textures
			if (material->GetTextureCount(aiTextureType_SHININESS) > 0)
			{
				if (!readAssimpTexture(material, aiTextureType_SHININESS, material_textures, material_transparent))
				{
					// free textures
					for (auto& material_textures : material_textures)
						material_textures.second->release();

					return false;
				}
			}

			// get ambient textures
			if (material->GetTextureCount(aiTextureType_AMBIENT) > 0)
			{
				if (!readAssimpTexture(material, aiTextureType_AMBIENT, material_textures, material_transparent))
				{
					// free textures
					for (auto& material_textures : material_textures)
						material_textures.second->release();

					return false;
				}
			}
			else if (app.video.getLighting() == Managers::Video::Lighting::PBR)
			{
				// add full ambient texture if there is no ambient texture
				material_textures.insert(material_textures.end(), std::make_pair("material.ambient", app.assets.getFullAmbientTexture()));
			}

			// get normal textures
			if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
			{
				if (!readAssimpTexture(material, aiTextureType_NORMALS, material_textures, material_transparent))
				{
					// free textures
					for (auto& material_textures : material_textures)
						material_textures.second->release();

					return false;
				}

				// set flag
				has_normal = true;
			}

			// get height textures
			if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
			{
				if (!readAssimpTexture(material, aiTextureType_HEIGHT, material_textures, material_transparent))
				{
					// free textures
					for (auto& material_textures : material_textures)
						material_textures.second->release();

					return false;
				}

				// set height map for normal map flag because obj file type stores normal maps that way
				has_normal = (m_file_extension == "obj");
			}

			// get displacement textures
			//if (material->GetTextureCount(aiTextureType_DISPLACEMENT) > 0)
			//{
			//	if (!readAssimpTexture(material, aiTextureType_DISPLACEMENT, material_textures, material_transparent))
			//	{
			//		// free textures
			//		for (auto& material_textures : material_textures)
			//			material_textures.second->release();

			//		return false;
			//	}

			//	// set flag
			//	has_displacement = true;
			//}

			// internal material shader
			Shader* material_shader = nullptr;

			// determine shader
			switch (app.video.getRendering())
			{
			case Managers::Video::Rendering::Forward:
				if (has_normal && has_displacement)
					material_shader = m_textured_parallax_shader;
				else if (has_normal)
					material_shader = m_textured_normal_shader;
				else
					material_shader = m_textured_shader;
				break;
			case Managers::Video::Rendering::Deferred:
				if (!material_transparent)
				{
					if (has_normal && has_displacement)
						material_shader = m_textured_parallax_shader;
					else if (has_normal)
						material_shader = m_textured_normal_shader;
					else
						material_shader = m_textured_shader;
				}
				else
				{
					if (has_normal && has_displacement)
						material_shader = m_textured_parallax_forward_shader;
					else if (has_normal)
						material_shader = m_textured_normal_forward_shader;
					else
						material_shader = m_textured_forward_shader;
				}
				break;
			}

			// make internal material
			out = new Material(material_shader, (!material_transparent) ? Material::Type::Lit : Material::Type::Transparent);

			// set blend factors
			/*if (material_transparent)
			{
				out->blend_src_factor = GL_SRC_ALPHA;
				out->blend_dst_factor = GL_ONE_MINUS_SRC_ALPHA;
			}*/

			// set textures
			out->setTextures(material_textures);

			// get shininess for blinn-phong
			if (app.video.getLighting() == Managers::Video::Lighting::BlinnPhong)
			{
				// specular shininess
				float shininess = 0.0f;

				// get shininess
				material->Get(AI_MATKEY_SHININESS, shininess);

				// set shininess
				out->setFloat("material.shininess", shininess);
			}

			// set displacement height
			if (has_displacement) out->setFloat("material.height", 0.01f);

			return true;
		}

		bool Model::readAssimpTexture(aiMaterial* material, aiTextureType type, std::vector<std::pair<std::string, Texture*>>& material_textures, bool& material_transparent)
		{
			// application
			System::Core& app = System::Core::getInstance();

			// texture sampler name in shader
			std::stringstream sampler_name;

			// get textures
			for (unsigned int i = 0; i < material->GetTextureCount(type); ++i)
			{
				// texture path
				aiString texture_path;

				// get texture path
				if (material->GetTexture(type, i, &texture_path) != aiReturn_SUCCESS)
				{
					app.logger.error("Failed to get material texture path from model!");

					return false;
				}

				if (std::string(texture_path.C_Str()) == "Sponza_Curtain_Blue_diffuse.tga")
				{
					int a = 0;
				}

				// full texture path
				std::string full_texture_path = std::string(m_directory).append(texture_path.C_Str());

				// texture loaded flag
				bool texture_loaded = false;

				// check if texture is loaded before
				for (auto texture : m_textures)
				{
					// if texture path is in loaded textures
					if (texture->getPath() == full_texture_path)
					{
						texture_loaded = true;

						// push to the list
						material_textures.insert(material_textures.end(), std::make_pair(texture->sampler, texture));

						// set material transparency
						material_transparent = (texture->type == Texture::Type::Transparent);

						// clear name
						sampler_name.str(std::string());

						break;
					}
				}

				// continue if texture is loaded before
				if (texture_loaded) continue;

				// diffuse flag
				bool is_diffuse = false;

				// get texture type
				switch (type)
				{
				case aiTextureType_DIFFUSE:
					material_transparent = (std::string(texture_path.C_Str()).find("_alpha.") != std::string::npos);

					is_diffuse = true;

					switch (app.video.getLighting())
					{
					case Managers::Video::Lighting::BlinnPhong:
						sampler_name << "material.diffuse";
						break;
					case Managers::Video::Lighting::PBR:
						sampler_name << "material.albedo";
						break;
					}
					break;
				case aiTextureType_SPECULAR:
					switch (app.video.getLighting())
					{
					case Managers::Video::Lighting::BlinnPhong:
						sampler_name << "material.specular";
						break;
					case Managers::Video::Lighting::PBR:
						sampler_name << "material.metallic";
						break;
					}
					break;
				case aiTextureType_AMBIENT:
					sampler_name << "material.ambient";
					break;
				case aiTextureType_EMISSIVE:
					sampler_name << "material.emissive";
					break;
				case aiTextureType_HEIGHT:
					sampler_name << ((m_file_extension != "obj") ? "material.height" : "material.normal");
					break;
				case aiTextureType_NORMALS:
					sampler_name << "material.normal";
					break;
				case aiTextureType_SHININESS:
					switch (app.video.getLighting())
					{
					case Managers::Video::Lighting::BlinnPhong:
						sampler_name << "material.shininess";
						break;
					case Managers::Video::Lighting::PBR:
						sampler_name << "material.roughness";
						break;
					}
					break;
				case aiTextureType_DISPLACEMENT:
					sampler_name << "material.displacement";
					break;
				case aiTextureType_LIGHTMAP:
					sampler_name << "material.light";
					break;
				case aiTextureType_REFLECTION:
					sampler_name << "material.reflection";
					break;
				}

				// add index to sampler name if there is more than one texture
				if (i > 0) sampler_name << i;

				// material texture
				Texture* texture = new Texture();
				texture->type = (!material_transparent) ? Texture::Type::Opaque : Texture::Type::Transparent;
				texture->sampler = sampler_name.str();

				// load texture
				if (!texture->load(full_texture_path.c_str(), false, false, 0, (!is_diffuse) ? 0 : 1, 0, 0))
				{
					// free texture
					texture->release();

					return false;
				}

				// push to the material textures list
				material_textures.insert(material_textures.end(), std::make_pair(texture->sampler, texture));

				// add to model textures
				m_textures.insert(m_textures.end(), texture);

				// clear name
				sampler_name.str(std::string());
			}

			return true;
		}

		void Model::releaseLoadingAssets()
		{
			// free entity
			if (m_entity)
			{
				m_entity->release();

				m_entity = nullptr;
			}

			// free textures
			for (auto asset : m_textures)
				asset->release();

			// free materials
			for (auto asset : m_materials)
				asset->release();

			// free meshes
			for (auto asset : m_meshes)
				asset->release();

			// clear lists
			m_textures.clear();
			m_materials.clear();
			m_meshes.clear();
		}
	}
}