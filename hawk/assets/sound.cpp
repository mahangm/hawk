#include "../system/core.hpp"
#include "sound.hpp"
#include <fmod_errors.h>
#include "../helpers/strings.hpp"
#include "../helpers/file_system.hpp"

namespace hawk
{
	namespace Assets
	{
		Sound::Sound() : m_sound(nullptr), m_mode(0)
		{
			// set asset type
			m_type = "Sound";
		}

		Sound::Sound(const Sound& other) : Asset(other), m_sound(other.m_sound) , m_mode(other.m_mode)
		{
			// set asset type
			m_type = "Sound";
		}

		Sound::~Sound() {}

		void Sound::operator=(const Sound& other)
		{
			m_sound = other.m_sound;
			m_mode = other.m_mode;
		}

		bool Sound::load(const std::string& path, int mode)
		{
			// application
			System::Core& app = System::Core::getInstance();

			// set asset path
			setPath(path);

			// fmod result
			FMOD_RESULT result;

			// load sound
			if ((result = app.audio.getLowLevelSystem()->createSound(path.c_str(), mode, nullptr, &m_sound)) != FMOD_OK)
			{
				app.logger.error("Failed to load sound!");
				app.logger.info(path.c_str());
				app.logger.info(FMOD_ErrorString(result));
			}

			// set sound mode
			m_mode = mode;

			return true;
		}

		void Sound::release()
		{
			// release sound source
			m_sound->release();

			// free object
			delete this;
		}

		FMOD::Sound* Sound::getSound() const
		{
			return m_sound;
		}
	}
}