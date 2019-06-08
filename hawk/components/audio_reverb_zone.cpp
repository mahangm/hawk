#include "../system/core.hpp"
#include "audio_reverb_zone.hpp"

namespace hawk
{
	namespace Components
	{
		AudioReverbZone::AudioReverbZone() : min_dist(10.0f), max_dist(20.0f), m_reverb(nullptr)
		{
			// set component type
			m_type = "AudioReverbZone";
		}

		AudioReverbZone::AudioReverbZone(const AudioReverbZone& other) : min_dist(other.min_dist), max_dist(other.max_dist), m_reverb(other.m_reverb)
		{
			// set component type
			m_type = "AudioReverbZone";
		}

		AudioReverbZone::AudioReverbZone(float min_dist, float max_dist) : min_dist(min_dist), max_dist(max_dist), m_reverb(nullptr)
		{
			// set component type
			m_type = "AudioReverbZone";
		}

		AudioReverbZone::~AudioReverbZone() {}

		void AudioReverbZone::operator=(const AudioReverbZone& other) 
		{
			min_dist = other.min_dist;
			max_dist = other.max_dist;
			m_reverb = other.m_reverb;
		}

		AudioReverbZone* AudioReverbZone::copy()
		{
			return new AudioReverbZone(*this);
		}

		void AudioReverbZone::sendMessage(const char* message)
		{
			// return if not enable
			if (!enable) return;

			// events
			if (std::strcmp(message, "Start") == 0)
				start();
			else if (std::strcmp(message, "Update") == 0)
				update();
		}

		void AudioReverbZone::release()
		{
			// release reverb zone
			if (m_reverb)
			{
				m_reverb->release();

				m_reverb = nullptr;
			}

			// free object
			delete this;
		}

		void AudioReverbZone::start()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// result
			FMOD_RESULT result;

			// make reverb zone
			if ((result = app.audio.getLowLevelSystem()->createReverb3D(&m_reverb)) != FMOD_OK)
			{
				app.logger.error("Failed to create audio reverb zone!");
				app.logger.info(entity->getName().c_str());

				return;
			}

			// reverb zone properties
			FMOD_REVERB_PROPERTIES reverb_prop = FMOD_PRESET_CONCERTHALL;

			// set reverb zone properties
			m_reverb->setProperties(&reverb_prop);
		}

		void AudioReverbZone::update()
		{
			// update zone properties
			if (m_reverb)
			{
				// channel position
				FMOD_VECTOR position = { entity->transform->getWorldPosition().x, entity->transform->getWorldPosition().y, entity->transform->getWorldPosition().z };

				// set zone attributes
				m_reverb->set3DAttributes(&position, min_dist, max_dist);
			}
		}

		FMOD::Reverb3D* AudioReverbZone::getReverb() const
		{
			return m_reverb;
		}
	}
}