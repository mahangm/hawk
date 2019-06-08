#include "../system/core.hpp"
#include "audio_player.hpp"
#include <fmod_errors.h>

namespace hawk
{
	namespace Components
	{
		AudioPlayer::AudioPlayer() : sound(nullptr), play_on_start(true), loop(false), m_channel(nullptr)
		{
			// set component type
			m_type = "AudioPlayer";
		}

		AudioPlayer::AudioPlayer(const AudioPlayer& other) : Component(other), 
			sound(other.sound), play_on_start(other.play_on_start), loop(other.loop), m_channel(other.m_channel)
		{
			// set component type
			m_type = "AudioPlayer";
		}

		AudioPlayer::AudioPlayer(Assets::Sound* sound, bool play_on_start, bool loop) : sound(sound), 
			play_on_start(play_on_start), loop(loop), m_channel(nullptr)
		{
			// set component type
			m_type = "AudioPlayer";
		}

		AudioPlayer::~AudioPlayer() {}

		void AudioPlayer::operator=(const AudioPlayer& other)
		{
			sound = other.sound;
			play_on_start = other.play_on_start;
			loop = other.loop;
			m_channel = other.m_channel;
		}

		AudioPlayer* AudioPlayer::copy()
		{
			return new AudioPlayer(*this);
		}

		void AudioPlayer::sendMessage(const char* message)
		{
			// return if not enable
			if (!enable) return;

			// events
			if (std::strcmp(message, "Start") == 0)
				start();
			else if (std::strcmp(message, "Update") == 0)
				update();
		}

		void AudioPlayer::release()
		{
			// free object
			delete this;
		}

		void AudioPlayer::start()
		{
			// play sound
			if (play_on_start) play();
		}

		void AudioPlayer::update()
		{
			// update channel attributes
			if (m_channel)
			{
				// channel position
				FMOD_VECTOR position = { entity->transform->getWorldPosition().x, entity->transform->getWorldPosition().y, entity->transform->getWorldPosition().z };

				// channel velocity
				FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };

				// set channel attributes
				m_channel->set3DAttributes(&position, &velocity);
			}
		}

		void AudioPlayer::play()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// fmod result
			FMOD_RESULT result;

			// play sound, paused because we need to set channel's position
			if ((result = app.audio.getLowLevelSystem()->playSound(sound->getSound(), nullptr, true, &m_channel)) != FMOD_OK)
			{
				app.logger.error("Failed to play sound!");
				app.logger.info(FMOD_ErrorString(result));
			}

			// channel position
			FMOD_VECTOR position = { entity->transform->getWorldPosition().x, entity->transform->getWorldPosition().y, entity->transform->getWorldPosition().z };

			// channel velocity
			FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };

			// set channel 3D attributes
			m_channel->set3DAttributes(&position, &velocity);

			// set channel playback mode
			if (loop)
				m_channel->setMode(FMOD_LOOP_NORMAL);
			else
				m_channel->setMode(FMOD_DEFAULT);

			// unpause to play sound at right position
			m_channel->setPaused(false);
		}

		void AudioPlayer::pause(bool paused)
		{
			// set channel paused state
			if (m_channel) m_channel->setPaused(paused);
		}

		void AudioPlayer::stop()
		{
			// stop channel
			if (m_channel) m_channel->stop();
		}

		bool AudioPlayer::isPlaying() const
		{
			// result
			bool is_playing = false;

			// get is playing
			if (m_channel->isPlaying(&is_playing) != FMOD_OK) return false;

			return is_playing;
		}

		bool AudioPlayer::isPaused() const
		{
			// result
			bool paused = false;

			// get paused
			if (m_channel->getPaused(&paused) != FMOD_OK) return false;

			return paused;
		}

		FMOD::Channel* AudioPlayer::getChannel() const
		{
			return m_channel;
		}
	}
}