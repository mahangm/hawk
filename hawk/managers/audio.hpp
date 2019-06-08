#ifndef HAWK_MANAGERS_AUDIO_H
#define HAWK_MANAGERS_AUDIO_H

// fmod
#include <fmod.hpp>
#include <fmod_studio.hpp>

// local
#include "manager.hpp"

namespace hawk
{
	namespace Managers
	{
		/**
		 * Audio manager.
		 */
		class Audio : public Manager
		{
		public:
			/**
			 * Constructor.
			 */
			Audio();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Audio(const Audio& other) = delete;

			/**
			 * Destructor.
			 */
			~Audio();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Audio& other) = delete;

			/**
			 * Initializer.
			 */
			bool initialize();

			/**
			 * Deinitializer.
			 */
			void deinitialize();

			/**
			 * Update FMOD system.
			 */
			void update();

			/**
			 * FMOD studio system.
			 */
			FMOD::Studio::System* getStudioSystem() const;

			/**
			 * FMOD low-level system.
			 */
			FMOD::System* getLowLevelSystem() const;

		private:
			/**
			 * FMOD studio system.
			 */
			FMOD::Studio::System* m_studio_system;

			/**
			 * FMOD low-level system.
			 */
			FMOD::System* m_low_level_system;
		};
	}
}
#endif