#ifndef HAWK_MANAGERS_TIME_H
#define HAWK_MANAGERS_TIME_H

// local
#include "manager.hpp"

namespace hawk
{
	namespace Managers
	{
		/**
		 * Time manager.
		 */
		class Time : public Manager
		{
		public:
			/**
			 * Constructor.
			 */
			Time();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Time(const Time& other) = delete;

			/**
			 * Destructor.
			 */
			~Time();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Time& other) = delete;

			/**
			 * Start time recorders.
			 */
			void start();

			/**
			 * Update fixed time recorders.
			 */
			void fixedUpdate();

			/**
			 * Update time recorders.
			 */
			void update();

			/**
			 * Current fixed frame time uneffected by time scale in seconds.
			 */
			float getFixedRealTime() const;

			/**
			 * Current fixed frame time in seconds.
			 */
			float getFixedCurrentTime() const;

			/**
			 * Current and previous fixed frame time difference in seconds.
			 */
			float getFixedDeltaTime() const;

			/**
			 * Fixed frame rate.
			 */
			float getFixedFrameRate() const;

			/**
			 * Current frame time uneffected by time scale in seconds.
			 */
			float getRealTime() const;

			/**
			 * Current frame time in seconds.
			 */
			float getCurrentTime() const;

			/**
			 * Current and previous frame time difference in seconds.
			 */
			float getDeltaTime() const;

			/**
			 * Frames per second.
			 */
			float getFps() const;

			/**
			 * Do we have fixed frames to consume?
			 */
			bool hasFixedFrames() const;

			/**
			 * Scale by what time will be scaled to.
			 */
			float time_scale;

		private:
			/**
			 * Current fixed frame time uneffected by time scale.
			 */
			float m_fixed_real_time;

			/**
			 * Current fixed frame time.
			 */
			float m_fixed_current_time;

			/**
			 * Last fixed frame time uneffected by time scale.
			 */
			float m_fixed_last_time;

			/**
			 * Difference between current fixed frame and previous fixed frame.
			 */
			float m_fixed_delta_time;

			/**
			 * Fixed frames rate per regular frame. e. g. 1 second / 50 frames.
			 */
			float m_fixed_frame_rate;

			/**
			 * Maximum fixed frame per regular frame to collect. e. g. 1 second / 30 frames.
			 */
			float m_fixed_frame_rate_max;

			/**
			 * Fixed frame collector.
			 */
			float m_fixed_frame_accumulator;

			/**
			 * Current frame time uneffected by time scale.
			 */
			float m_real_time;

			/**
			 * Current frame time.
			 */
			float m_current_time;

			/**
			 * Last frame time uneffected by time scale.
			 */
			float m_last_time;

			/**
			 * Difference between current frame and previous frame.
			 */
			float m_delta_time;

			/**
			 * Frames count.
			 */
			unsigned int m_frames;

			/**
			 * Frames per second.
			 */
			float m_fps;

			/**
			 * Frames per second timer.
			 */
			float m_fps_time;
		};
	}
}
#endif