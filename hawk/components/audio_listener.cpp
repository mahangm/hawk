#include "../system/core.hpp"
#include "audio_listener.hpp"

namespace hawk
{
	namespace Components
	{
		AudioListener::AudioListener() : m_attributes({ { 0 } }), m_last_attributes({ { 0 } }), m_listener_id(0)
		{
			// set component type
			m_type = "AudioListener";
		}

		AudioListener::AudioListener(const AudioListener& other) : Component(other), 
			m_attributes(other.m_attributes), m_last_attributes(other.m_last_attributes), m_listener_id(other.m_listener_id)
		{
			// set component type
			m_type = "AudioListener";
		}

		AudioListener::~AudioListener() {}

		void AudioListener::operator=(const AudioListener& other) 
		{
			m_attributes = other.m_attributes;
			m_last_attributes = other.m_last_attributes;
			m_listener_id = other.m_listener_id;
		}

		AudioListener* AudioListener::copy()
		{
			return new AudioListener(*this);
		}

		void AudioListener::sendMessage(const char* message)
		{
			// return if not enable
			if (!enable) return;

			// events
			if (std::strcmp(message, "Start") == 0)
				start();
			else if (std::strcmp(message, "Update") == 0)
				update();
		}

		void AudioListener::release()
		{
			// free object
			delete this;
		}

		void AudioListener::start()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// set position
			m_attributes.position.x = entity->transform->getWorldPosition().x;
			m_attributes.position.y = entity->transform->getWorldPosition().y;
			m_attributes.position.z = entity->transform->getWorldPosition().z;

			// set orientation
			m_attributes.forward.x = app.video.camera->front.x;
			m_attributes.forward.y = app.video.camera->front.y;
			m_attributes.forward.z = app.video.camera->front.z;
			m_attributes.up.x = app.video.camera->up.x;
			m_attributes.up.y = app.video.camera->up.y;
			m_attributes.up.z = app.video.camera->up.z;

			// set last attributes
			m_last_attributes = m_attributes;

			// set listener attributes
			app.audio.getStudioSystem()->setListenerAttributes(m_listener_id, &m_attributes);
		}

		void AudioListener::update()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// set position
			m_attributes.position.x = entity->transform->getWorldPosition().x;
			m_attributes.position.y = entity->transform->getWorldPosition().y;
			m_attributes.position.z = entity->transform->getWorldPosition().z;

			// set velocity indipendent of frame rate by scaling it to meters per second not meters per frame for fmod
			m_attributes.velocity.x = (m_attributes.velocity.x - m_last_attributes.velocity.x) * (1.0f / app.time.getDeltaTime());
			m_attributes.velocity.y = (m_attributes.velocity.y - m_last_attributes.velocity.y) * (1.0f / app.time.getDeltaTime());
			m_attributes.velocity.z = (m_attributes.velocity.z - m_last_attributes.velocity.z) * (1.0f / app.time.getDeltaTime());

			// set orientation
			m_attributes.forward.x = app.video.camera->front.x;
			m_attributes.forward.y = app.video.camera->front.y;
			m_attributes.forward.z = app.video.camera->front.z;
			m_attributes.up.x = app.video.camera->up.x;
			m_attributes.up.y = app.video.camera->up.y;
			m_attributes.up.z = app.video.camera->up.z;

			// set last attributes
			m_last_attributes = m_attributes;

			// set listener attributes
			app.audio.getStudioSystem()->setListenerAttributes(m_listener_id, &m_attributes);
		}

		int AudioListener::getListenerId() const
		{
			return m_listener_id;
		}
	}
}