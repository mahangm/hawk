#ifndef HAWK_MANAGERS_INPUT_H
#define HAWK_MANAGERS_INPUT_H

// glfw
#include <GLFW/glfw3.h>

// local
#include "manager.hpp"

namespace hawk
{
	namespace Managers
	{
		/**
		 * Input manager.
		 */
		class Input : public Manager
		{
		public:
			/**
			 * Constructor.
			 */
			Input();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Input(const Input& other) = delete;

			/**
			 * Destructor.
			 */
			~Input();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Input& other) = delete;

			/**
			 * Initializer.
			 * @param window Input window.
			 * @return True or False.
			 */
			bool initialize(GLFWwindow* window);

			/**
			 * Update keys states.
			 */
			void update();

			/**
			 * Is key down?
			 * @param key Key code.
			 * @return True or False.
			 */
			bool keyDown(int key);

			/**
			 * Is key pressed?
			 * @param key Key code.
			 * @return True or False.
			 */
			bool keyPress(int key);

			/**
			 * Is key released?
			 * @param key Key code.
			 * @return True or False.
			 */
			bool keyUp(int key);

			/**
			 * Is mouse button down?
			 * @param button Button code.
			 * @return True or False.
			 */
			bool buttonDown(int button);

			/**
			 * Is mouse button pressed?
			 * @param button Button code.
			 * @return True or False.
			 */
			bool buttonPress(int button);

			/**
			 * Is mouse button released?
			 * @param button Button code.
			 * @return True or False.
			 */
			bool buttonUp(int button);

			/**
			 * Mouse move callback type.
			 */
			typedef void(MouseMoveCallback)(double x, double y);

			/**
			 * Mouse scroll callback type.
			 */
			typedef void(MouseScrollCallback)(double x, double y);

			/**
			 * Mouse move event.
			 */
			MouseMoveCallback* onMouseMove;

			/**
			 * Mouse scroll event.
			 */
			MouseScrollCallback* onMouseScroll;

			/**
			 * Mouse cursor x position.
			 */
			float x;

			/**
			 * Mouse cursor y position.
			 */
			float y;

			/**
			 * Mouse wheel position.
			 */
			float z;

		private:
			/**
			 * Mouse move callback.
			 */
			static void mouseMoveCallback(GLFWwindow* window, double x, double y);

			/**
			 * Mouse scroll callback.
			 */
			static void mouseScrollCallback(GLFWwindow* window, double x, double y);

			/**
			 * Input window.
			 */
			GLFWwindow* m_window;

			/**
			 * Keys states.
			 */
			char m_keys[512];

			/**
			 * Keys down states.
			 */
			char m_keys_down[512];

			/**
			 * Keys up states.
			 */
			char m_keys_up[512];

			/**
			 * Buttons states.
			 */
			char m_buttons[8];

			/**
			 * Buttons down states.
			 */
			char m_buttons_down[8];

			/**
			 * Buttons up states.
			 */
			char m_buttons_up[8];
		};
	}
}
#endif