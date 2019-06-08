#ifndef HAWK_MANAGERS_ASSETS_H
#define HAWK_MANAGERS_ASSETS_H

// stl
#include <map>

// local
#include "manager.hpp"
#include "../assets/shader.hpp"
#include "../assets/texture.hpp"
#include "../assets/material.hpp"
#include "../assets/mesh.hpp"
#include "../assets/model.hpp"
#include "../assets/sound.hpp"

namespace hawk
{
	namespace Managers
	{
		/**
		 * Asset manager.
		 */
		class Assets : public Manager
		{
		public:
			/**
			 * Constructor.
			 */
			Assets();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Assets(const Assets& other) = delete;

			/**
			 * Destructor.
			 */
			~Assets();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Assets& other) = delete;

			/**
			 * Initializer.
			 */
			bool initialize();

			/**
			 * Deinitialize.
			 */
			void deinitialize();

			/**
			 * Add shader.
			 * @param name Asset name.
			 * @param paths Shader paths. The order is vertex, fragment, geometry shader.
			 * @return Pointer to asset otherwise null pointer.
			 */
			hawk::Assets::Shader* addShader(const char* name, const std::vector<std::string>& paths);

			/**
			 * Add a list of pre-made shaders.
			 * @param shaders List of shaders.
			 */
			void addShaders(const std::map<std::string, hawk::Assets::Shader*>& shaders);

			/**
			 * Remove shader.
			 * @param name Asset name.
			 */
			void removeShader(const char* name);

			/**
			 * Add 2D texture.
			 * @param name Asset name.
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
			 * @return Pointer to asset otherwise null pointer.
			 */
			hawk::Assets::Texture* addTexture(const char* name, const std::string& path, 
				bool flip = true, bool hdr = false, int mips = 0,
				int internal_format = 0, unsigned int data_format = 0, unsigned int data_type = 0,
				int wrap = GL_REPEAT, int min_filter = GL_LINEAR_MIPMAP_LINEAR, int mag_filter = GL_LINEAR);

			/**
			 * Add cube texture.
			 * @param name Asset name.
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
			 * @return Pointer to asset otherwise null pointer.
			 */
			hawk::Assets::Texture* addTexture(const char* name, const std::vector<std::string>& paths,
				bool flip = true, bool hdr = false, int mips = 0,
				int internal_format = 0, unsigned int data_format = 0, unsigned int data_type = 0,
				int wrap = GL_REPEAT, int min_filter = GL_LINEAR_MIPMAP_LINEAR, int mag_filter = GL_LINEAR);

			/**
			 * Add a list of pre-made textures.
			 * @param textures List of textures.
			 */
			void addTextures(const std::map<std::string, hawk::Assets::Texture*>& textures);

			/**
			 * Remove texture.
			 * @param name Asset name.
			 */
			void removeTexture(const char* name);

			/**
			 * Add material.
			 * @param name Asset name.
			 * @param shader Material shader.
			 * @param type Material type.
			 * @return Pointer to asset otherwise null pointer.
			 */
			hawk::Assets::Material* addMaterial(const char* name, hawk::Assets::Shader* shader, hawk::Assets::Material::Type type = hawk::Assets::Material::Type::None);

			/**
			 * Add a list of pre-made materials.
			 * @param materials List of materials.
			 */
			void addMaterials(const std::map<std::string, hawk::Assets::Material*>& materials);

			/**
			 * Remove material.
			 * @param name Asset name.
			 */
			void removeMaterial(const char* name);

			/**
			 * Add mesh.
			 * @param name Asset name.
			 * @param primitive Primitive shape type.
			 * @param width Mesh width in normalized device coordinates.
			 * @param height Mesh height in normalized device coordinates.
			 * @param length Mesh length in normalized device coordinates.
			 * @return Pointer to asset otherwise null pointer.
			 */
			hawk::Assets::Mesh* addMesh(const char* name, hawk::Assets::Mesh::Primitive primitive, float width = 1.0f, float height = 1.0f, float length = 1.0f);

			/**
			 * Add a list of pre-made meshes.
			 * @param meshes List of meshes.
			 */
			void addMeshes(const std::map<std::string, hawk::Assets::Mesh*>& meshes);

			/**
			 * Remove mesh.
			 * @param name Asset name.
			 */
			void removeMesh(const char* name);

			/**
			 * Add model.
			 * @param name Asset name.
			 * @param path Model file path.
			 * @return Pointer to asset otherwise null pointer.
			 */
			hawk::Assets::Model* addModel(const char* name, const std::string& path);

			/**
			 * Add a list of pre-made models.
			 * @param models List of models.
			 */
			void addModels(const std::map<std::string, hawk::Assets::Model*>& models);

			/**
			 * Remove model.
			 * @param name Asset name.
			 */
			void removeModel(const char* name);

			/**
			 * Add sound.
			 * @param name Asset name.
			 * @param path Sound file path.
			 * @param mode Sound mode.
			 * @return Pointer to asset otherwise null pointer.
			 */
			hawk::Assets::Sound* addSound(const char* name, const std::string& path, int mode = FMOD_3D | FMOD_CREATESAMPLE);

			/**
			 * Add a list of pre-made sounds.
			 * @param sounds List of sounds.
			 */
			void addSounds(const std::map<std::string, hawk::Assets::Sound*>& sounds);

			/**
			 * Remove sound.
			 * @param name Asset name.
			 */
			void removeSound(const char* name);

			/**
			 * Release assets.
			 */
			void release();

			/**
			 * BRDF look up texture.
			 */
			hawk::Assets::Texture* getBRDFTexture() const;

			/**
			 * Zero metallic texture for objects without metallic texture.
			 */
			hawk::Assets::Texture* getZeroMetallicTexture() const;

			/**
			 * Zero roughness texture for objects without roughness texture.
			 */
			hawk::Assets::Texture* getZeroRoughnessTexture() const;

			/**
			 * Full metallic texture for objects without metallic texture.
			 */
			hawk::Assets::Texture* getFullMetallicTexture() const;

			/**
			 * Full roughness texture for objects without roughness texture.
			 */
			hawk::Assets::Texture* getFullRoughnessTexture() const;

			/**
			 * Full ambient texture for objects without ambient texture.
			 */
			hawk::Assets::Texture* getFullAmbientTexture() const;

			/**
			 * Shaders list.
			 */
			std::map<std::string, hawk::Assets::Shader*> shaders;

			/**
			 * Textures list.
			 */
			std::map<std::string, hawk::Assets::Texture*> textures;

			/**
			 * Materials list.
			 */
			std::map<std::string, hawk::Assets::Material*> materials;

			/**
			 * Meshes list.
			 */
			std::map<std::string, hawk::Assets::Mesh*> meshes;

			/**
			 * Models list.
			 */
			std::map<std::string, hawk::Assets::Model*> models;

			/**
			 * Sounds list.
			 */
			std::map<std::string, hawk::Assets::Sound*> sounds;

		private:
			/**
			 * Shaders id holder.
			 */
			unsigned int m_shaders_id;

			/**
			 * Textures id holder.
			 */
			unsigned int m_textures_id;

			/**
			 * Materials id holder.
			 */
			unsigned int m_materials_id;

			/**
			 * Meshes id holder.
			 */
			unsigned int m_meshes_id;

			/**
			 * Models id holder.
			 */
			unsigned int m_models_id;

			/**
			 * Sounds id holder.
			 */
			unsigned int m_sounds_id;

			/**
			 * BRDF look up texture.
			 */
			hawk::Assets::Texture* m_brdf_texture;

			/**
			 * Zero metallic texture for objects without metallic texture.
			 */
			hawk::Assets::Texture* m_zero_metallic_texture;

			/**
			 * Zero roughness texture for objects without roughness texture.
			 */
			hawk::Assets::Texture* m_zero_roughness_texture;

			/**
			 * Full metallic texture for objects without metallic texture.
			 */
			hawk::Assets::Texture* m_full_metallic_texture;

			/**
			 * Full roughness texture for objects without roughness texture.
			 */
			hawk::Assets::Texture* m_full_roughness_texture;

			/**
			 * Full ambient texture for objects without ambient texture.
			 */
			hawk::Assets::Texture* m_full_ambient_texture;
		};
	}
}
#endif