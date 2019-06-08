#ifndef HAWK_HELPERS_MATH_H
#define HAWK_HELPERS_MATH_H

// stl
#include <cmath>
#include <limits>
#include <random>

namespace hawk
{
	namespace Helpers
	{
		namespace Math
		{
			/**
			 * Pi number.
			 */
			constexpr double PI = 3.14159265358979;

			/**
			 * Epsilon number.
			 */
			constexpr float EPSILON = std::numeric_limits<float>::epsilon();

			/**
			 * Random device.
			 */
			std::random_device rand_device;

			/**
			 * Random engine.
			 */
			std::mt19937 rand_engine(rand_device());

			/**
			 * Clamp value between min and max.
			 */
			inline float clamp(float value, float min, float max)
			{
				return value < min ? min : (value > max ? max : value);
			}

			/**
			 * Clamp value between 0 and 1.
			 */
			inline float clamp01(float value)
			{
				return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
			}

			/**
			 * Float mod.
			 */
			inline float mod(float value, float divide)
			{
				float ratio = value / divide;

				return divide * (ratio - std::floor(ratio));
			}

			/**
			 * Lerp.
			 */
			inline float lerp(float from, float to, float t)
			{
				return from + (to - from) * clamp01(t / ((from > to) ? from : to));
			}

			/**
			 * Smooth lerp.
			 */
			inline float slerp(float from, float to, float t)
			{
				float mu = (float)(1 - std::cos(t * (float)PI)) / 2;

				return (from * (1 - mu) + to * mu);
			}

			/**
			 * Ping pong.
			 */
			inline float pingPong(float t, float length)
			{
				t = mod(t, 2 * length);

				if (t < length) return t; else return 2 * length - t;
			}

			/**
			 * Approximate equallity of floats.
			 */
			inline float approximatelyEqual(float a, float b)
			{
				return std::fabs(a - b) <= ((std::fabs(a) < std::fabs(b) ? std::fabs(b) : std::fabs(a)) * EPSILON);
			}

			/**
			 * Essential equallity of floats.
			 */
			inline float essentiallyEqual(float a, float b)
			{
				return std::fabs(a - b) <= ((std::fabs(a) > std::fabs(b) ? std::fabs(b) : std::fabs(a)) * EPSILON);
			}

			/**
			 * Definitley greater than of floats.
			 */
			inline float definitleyGreaterThan(float a, float b)
			{
				return (a - b) > ((std::fabs(a) < std::fabs(b) ? std::fabs(b) : std::fabs(a)) * EPSILON);
			}

			/**
			 * Definitley less than of floats.
			 */
			inline float definitleyLessThan(float a, float b)
			{
				return (b - a) > ((std::fabs(a) < std::fabs(b) ? std::fabs(b) : std::fabs(a)) * EPSILON);
			}

			/**
			 * Random float.
			 */
			inline float random()
			{
				// 0 to 1 distribution
				std::uniform_real_distribution<float> dist(0.0f, 1.0f);

				return dist(rand_engine);
			}

			/**
			 * Random range float.
			 */
			inline float random(float min, float max)
			{
				// return if same
				if (min == max) { return min; }

				// min to max distribution
				std::uniform_real_distribution<float> dist(min, max);

				return dist(rand_engine);
			}
		}
	}
}
#endif