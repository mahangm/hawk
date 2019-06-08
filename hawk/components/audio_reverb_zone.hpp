#ifndef HAWK_COMPONENTS_AUDIO_REVERB_ZONE_H
#define HAWK_COMPONENTS_AUDIO_REVERB_ZONE_H

// fmod
#include <fmod.hpp>

// local
#include "component.hpp"

namespace hawk
{
	namespace Components
	{
		/**
		 * Audio reverb zone component.
		 */
		class AudioReverbZone : public Component
		{
		public:
			/**
			 * Constructor.
			 */
			AudioReverbZone();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			AudioReverbZone(const AudioReverbZone& other);

			/**
			 * Constructor.
			 * @param min_dist Reverb zone sphere minimum distance.
			 * @param max_dist Reverb zone sphere maximum distance.
			 */
			AudioReverbZone(float min_dist, float max_dist);

			/**
			 * Destructor.
			 */
			~AudioReverbZone();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const AudioReverbZone& other);

			/**
			 * Make a copy from this object.
			 */
			AudioReverbZone* copy();

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
			 * Reverb zone.
			 */
			FMOD::Reverb3D* getReverb() const;

			/**
			 * Reverb zone sphere minimum distance.
			 */
			float min_dist;

			/**
			 * Reverb zone sphere maximum distance.
			 */
			float max_dist;

		private:
			/**
			 * Reverb zone.
			 */
			FMOD::Reverb3D* m_reverb;
		};
	}
}
#endif