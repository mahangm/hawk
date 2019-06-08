#ifndef HAWK_ASSETS_MODEL_H
#define HAWK_ASSETS_MODEL_H

// stl
#include <map>

// assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// local
#include "asset.hpp"
#include "texture.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "../objects/entity.hpp"
#include "../components/mesh_renderer.hpp"

namespace hawk
{
	namespace Assets
	{
		/**
		 * Model asset.
		 */
		class Model : public Asset
		{
		public:
			/**
			 * Constructor.
			 */
			Model();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Model(const Model& other);

			/**
			 * Destructor.
			 */
			~Model();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Model& other);

			/**
			 * Load model from file.
			 * @param path Model file path.
			 * @return True or False.
			 */
			bool load(const std::string& path);

			/**
			 * Release resources.
			 */
			void release();

			/**
			 * Model entity.
			 */
			Objects::Entity* getEntity() const;

		private:
			/**
			 * Read assimp node recursively.
			 * @param node Assimp node.
			 * @param scene Assimp scene.
			 * @param root_entity Root entity.
			 * @return True or False;
			 */
			bool readAssimpNode(aiNode* node, const aiScene* scene, Objects::Entity* root_entity);

			/**
			 * Read a mesh from model.
			 * @param mesh Assimp mesh.
			 * @param scene Assimp scene.
			 * @param out Hawk mesh.
			 * @return True or False;
			 */
			bool readAssimpMesh(aiMesh* mesh, const aiScene* scene, Mesh*& out);

			/**
			 * Read a material from model.
			 * @param material Assimp material.
			 * @param scene Assimp scene.
			 * @param out Hawk material.
			 * @return True or False.
			 */
			bool readAssimpMaterial(aiMaterial* material, const aiScene* scene, Material*& out);

			/**
			 * Read a material's textures from model.
			 * @param material Assimp material.
			 * @param type Assimp texture type.
			 * @param material_textures List of hawk textures for hawk material.
			 * @param material_transparent Returns true if texture is transparent.
			 * @return True or False.
			 */
			bool readAssimpTexture(aiMaterial* material, aiTextureType type, std::vector<std::pair<std::string, Texture*>>& material_textures, bool& material_transparent);

			/**
			 * Release generated assets while reading model.
			 */
			void releaseLoadingAssets();

			/**
			 * Assimp importer.
			 */
			Assimp::Importer* m_importer;

			/**
			 * Assimp scene.
			 */
			const aiScene* m_scene;

			/**
			 * Model textures.
			 */
			std::vector<Texture*> m_textures;

			/**
			 * Model materials.
			 */
			std::vector<Material*> m_materials;

			/**
			 * Model meshes.
			 */
			std::vector<Mesh*> m_meshes;

			/**
			 * Model entity.
			 */
			Objects::Entity* m_entity;

			/**
			 * Textured shader.
			 */
			Shader* m_textured_shader;

			/**
			 * Textured normal shader.
			 */
			Shader* m_textured_normal_shader;

			/**
			 * Textured parallax shader.
			 */
			Shader* m_textured_parallax_shader;

			/**
			 * Textured forward shader for deferred transparency.
			 */
			Shader* m_textured_forward_shader;

			/**
			 * Textured normal forward shader for deferred transparency.
			 */
			Shader* m_textured_normal_forward_shader;

			/**
			 * Textured parallax forward shader for deferred transparency.
			 */
			Shader* m_textured_parallax_forward_shader;
		};
	}
}
#endif