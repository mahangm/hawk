#ifndef HAWK_COMPONENTS_AUDIO_PLAYER_H
#define HAWK_COMPONENTS_AUDIO_PLAYER_H

// fmod
#include <fmod.hpp>

// local
#include "component.hpp"
#include "../assets/sound.hpp"

namespace hawk
{
	/**
	 * Audio player component.
	 */
	namespace Components
	{
		class AudioPlayer : public Component
		{
		public:
			/**
			 * Constructor.
			 */
			AudioPlayer();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			AudioPlayer(const AudioPlayer& other);

			/**
			 * Constructor.
			 * @param sound Audio player sound asset.
			 * @param play_on_start Play sound on start.
			 * @param loop Loop mode.
			 */
			AudioPlayer(Assets::Sound* sound, bool play_on_start = true, bool loop = false);

			/**
			 * Destructor.
			 */
			~AudioPlayer();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const AudioPlayer& other);

			/**
			 * Make a copy from this object.
			 */
			AudioPlayer* copy();

			/**
			 * Send message to trigger events.
			 * @param message Message context.
			 */
			void sendMessage(const char* message);

			/**
			 * Release resources.
			 */
			void release();

			/**
			 * Start.
			 */
			void start();

			/**
			 * Update.
			 */
			void update();

			/**
			 * Play sound.
			 */
			void play();

			/**
			 * Pause sound.
			 * @param paused True or False.
			 */
			void pause(bool paused);

			/**
			 * Stop sound.
			 */
			void stop();

			/**
			 * Is sound playing?
			 * @return True or False.
			 */
			bool isPlaying() const;

			/**
			 * Is sound paused?
			 * @return True or False.
			 */
			bool isPaused() const;

			/**
			 * FMOD channel.
			 */
			FMOD::Channel* getChannel() const;

			/**
			 * Sound asset.
			 */
			Assets::Sound* sound;

			/**
			 * Play on start.
			 */
			bool play_on_start;

			/**
			 * Loop mode.
			 */
			bool loop;

		private:
			/**
			 * FMOD channel.
			 */
			FMOD::Channel* m_channel;
		};
	}
}
#endif