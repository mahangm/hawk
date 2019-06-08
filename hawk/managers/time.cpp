#include "time.hpp"
#include <GLFW/glfw3.h>

namespace hawk
{
	namespace Managers
	{
		Time::Time() : time_scale(1.0f), m_fixed_real_time(0.0f), m_fixed_current_time(0.0f), m_fixed_last_time(0.0f), m_fixed_delta_time(0.0f),
			m_fixed_frame_rate(1.0f / 50.0f), m_fixed_frame_rate_max(1.0f / 30.0f), m_fixed_frame_accumulator(0.0f), 
			m_real_time(0.0f), m_current_time(0.0f), m_last_time(0.0f), m_delta_time(0.0f), m_frames(0), m_fps(0.0f), m_fps_time(0.0f)
		{
			// set manager name
			m_name = "Time";
		}

		Time::~Time() {}

		void Time::start()
		{
			// set times
			m_fixed_real_time = m_fixed_last_time = m_real_time = m_last_time = m_fps_time = (float)glfwGetTime();
		}

		void Time::fixedUpdate()
		{
			// set fixed real time
			m_fixed_real_time = (float)glfwGetTime();

			// set fixed current time
			m_fixed_current_time = m_fixed_real_time * time_scale;

			// set fixed delta time
			m_fixed_delta_time = (m_fixed_real_time - m_fixed_last_time) * time_scale;

			// set last fixed frame time
			m_fixed_last_time = m_fixed_real_time;

			// consume fixed frames
			m_fixed_frame_accumulator -= m_fixed_frame_rate;
		}

		void Time::update()
		{
			// set real time
			m_real_time = (float)glfwGetTime();

			// set current time
			m_current_time = m_real_time * time_scale;

			// delta time
			float delta_time = m_real_time - m_last_time;

			// set delta time
			m_delta_time = delta_time * time_scale;

			// set last frame time
			m_last_time = m_real_time;

			// collect fixed frames
			m_fixed_frame_accumulator += (delta_time > m_fixed_frame_rate_max) ? m_fixed_frame_rate_max : delta_time;

			// collect frames
			m_frames++;

			// if fps time reached one second
			if (m_real_time - m_fps_time >= 1.0f)
			{
				// set fps
				m_fps = (float)m_frames;

				//m_fps = (float)(1000.0 / (double)m_frames); // needed time to draw a frame in millisecond (better for anlaysis)

				// reset frames count
				m_frames = 0;

				// set fps time
				m_fps_time += 1.0f;
			}
		}

		float Time::getFixedRealTime() const
		{
			return m_fixed_real_time;
		}

		float Time::getFixedCurrentTime() const
		{
			return m_fixed_current_time;
		}

		float Time::getFixedDeltaTime() const
		{
			return m_fixed_delta_time;
		}

		float Time::getFixedFrameRate() const
		{
			return m_fixed_frame_rate;
		}

		float Time::getRealTime() const
		{
			return m_real_time;
		}

		float Time::getCurrentTime() const
		{
			return m_current_time;
		}

		float Time::getDeltaTime() const
		{
			return m_delta_time;
		}

		float Time::getFps() const
		{
			return m_fps;
		}

		bool Time::hasFixedFrames() const
		{
			return (m_fixed_frame_accumulator >= m_fixed_frame_rate);
		}
	}
}