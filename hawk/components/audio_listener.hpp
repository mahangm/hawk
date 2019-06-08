#ifndef HAWK_COMPONENTS_AUDIO_LISTENER_H
#define HAWK_COMPONENTS_AUDIO_LISTENER_H

// fmod
#include <fmod_common.h>

// local
#include "component.hpp"

namespace hawk
{
	namespace Components
	{
		/**
		 * Audio listener component.
		 */
		class AudioListener : public Component
		{
		public:
			/**
			 * Constructor.
			 */
			AudioListener();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			AudioListener(const AudioListener& other);

			/**
			 * Destructor.
			 */
			~AudioListener();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const AudioListener& other);

			/**
			 * Make a copy from this object.
			 */
			AudioListener* copy();

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
			 * Listener id.
			 */
			int getListenerId() const;

		private:
			/**
			 * Listener attributes.
			 */
			FMOD_3D_ATTRIBUTES m_attributes;

			/**
			 * Listener last frame attributes.
			 */
			FMOD_3D_ATTRIBUTES m_last_attributes;

			/**
			 * Listener id.
			 */
			int m_listener_id;
		};
	}
}
#endif