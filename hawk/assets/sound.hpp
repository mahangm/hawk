#ifndef HAWK_ASSETS_SOUND_H
#define HAWK_ASSETS_SOUND_H

// fmod
#include <fmod.hpp>

// local
#include "asset.hpp"

namespace hawk
{
	namespace Assets
	{
		class Sound : public Asset
		{
		public:
			/**
			 * Compression enum.
			 */
			enum class Compression
			{
				Decompressed = 1,
				Streamed,
				Compressed
			};

			/**
			 * Constructor.
			 */
			Sound();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Sound(const Sound& other);

			/**
			 * Destructor.
			 */
			~Sound();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Sound& other);

			/**
			 * Load sound from file.
			 * @param path Sound file path.
			 * @param mode Sound mode.
			 * @return True or False.
			 */
			bool load(const std::string& path, int mode = FMOD_3D | FMOD_CREATESAMPLE);

			/**
			 * Release resources.
			 */
			void release();

			/**
			 * FMOD sound.
			 */
			FMOD::Sound* getSound() const;

		private:
			/**
			 * FMOD sound.
			 */
			FMOD::Sound* m_sound;

			/**
			 * Sound mode. 2D or 3D.
			 */
			int m_mode;
		};
	}
}
#endif