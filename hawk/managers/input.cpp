#include "../system/core.hpp"
#include "input.hpp"
#include "../helpers/memory.hpp"

namespace hawk
{
	namespace Managers
	{
		Input::Input() : onMouseMove(nullptr), onMouseScroll(nullptr), x(0.0f), y(0.0f), z(0.0f), m_window(nullptr)
		{
			// set manager name
			m_name = "Input";

			// zeroize
			hawk::Helpers::Memory::zeroize(m_keys, sizeof m_keys);
			hawk::Helpers::Memory::zeroize(m_keys_down, sizeof m_keys_down);
			hawk::Helpers::Memory::zeroize(m_keys_up, sizeof m_keys_up);
			hawk::Helpers::Memory::zeroize(m_buttons, sizeof m_buttons);
			hawk::Helpers::Memory::zeroize(m_buttons_down, sizeof m_buttons_down);
			hawk::Helpers::Memory::zeroize(m_buttons_up, sizeof m_buttons_up);
		}

		Input::~Input() {}

		bool Input::initialize(GLFWwindow* window)
		{
			// set mouse move callback
			glfwSetCursorPosCallback(window, Input::mouseMoveCallback);

			// set mouse scroll callback
			glfwSetScrollCallback(window, Input::mouseScrollCallback);

			// set input window
			m_window = window;

			// set keys states
			for (int i = 0; i < 512; ++i)
			{
				m_keys[i] = -1;
				m_keys_down[i] = 0;
				m_keys_up[i] = 0;
			}

			// set buttons states
			for (int i = 0; i < 8; ++i)
			{
				m_buttons[i] = -1;
				m_buttons_down[i] = 0;
				m_buttons_up[i] = 0;
			}

			return true;
		}

		void Input::update()
		{
			// set keys states
			for (int i = 0; i < 512; ++i)
			{
				// check keys down
				if (m_keys_down[i] == 1 && m_keys[i] == GLFW_RELEASE)
				{
					m_keys_down[i] = 0;
					m_keys_up[i] = 1;
				}

				m_keys[i] = -1;
			}

			// set buttons states
			for (int i = 0; i < 8; ++i)
			{
				// check buttons down
				if (m_buttons_down[i] == 1 && m_buttons[i] == GLFW_RELEASE)
				{
					m_buttons_down[i] = 0;
					m_buttons_up[i] = 1;
				}

				m_buttons[i] = -1;
			}
		}

		bool Input::keyDown(int key)
		{
			// store key state
			m_keys[key] = glfwGetKey(m_window, key);

			// key down result
			bool is_key_down = false;

			// check key down
			if (m_keys_down[key] == 0 && m_keys[key] == GLFW_PRESS)
			{
				// set result
				is_key_down = true;

				// flag this key
				m_keys_down[key] = 1;
			}

			return is_key_down;
		}

		bool Input::keyPress(int key)
		{
			// store key state and return result
			return ((m_keys[key] = glfwGetKey(m_window, key)) == GLFW_PRESS);
		}

		bool Input::keyUp(int key)
		{
			// store key state
			m_keys[key] = glfwGetKey(m_window, key);

			// set key down if key is pressed
			if (m_keys[key] == GLFW_PRESS) m_keys_down[key] = 1;

			// key up result
			bool is_key_up = (m_keys_up[key] == 1);

			// reset key up
			m_keys_up[key] = 0;

			return is_key_up;
		}

		bool Input::buttonDown(int button)
		{
			// store button state
			m_buttons[button] = glfwGetMouseButton(m_window, button);

			// button down result
			bool is_button_down = false;

			// check button down
			if (m_buttons_down[button] == 0 && m_buttons[button] == GLFW_PRESS)
			{
				// set result
				is_button_down = true;

				// flag this button
				m_buttons_down[button] = 1;
			}

			return is_button_down;
		}

		bool Input::buttonPress(int button)
		{
			// store button state and return result
			return ((m_buttons[button] = glfwGetMouseButton(m_window, button)) == GLFW_PRESS);
		}

		bool Input::buttonUp(int button)
		{
			// store button state
			m_buttons[button] = glfwGetMouseButton(m_window, button);

			// set button down if button is pressed
			if (m_buttons[button] == GLFW_PRESS) m_buttons_down[button] = 1;

			// button up result
			bool is_button_up = (m_buttons_up[button] == 1);

			// reset button up
			m_buttons_up[button] = 0;

			return is_button_up;
		}

		void Input::mouseMoveCallback(GLFWwindow* window, double x, double y)
		{
			// set mouse cursor position
			System::Core::getInstance().input.x = (float)x;
			System::Core::getInstance().input.y = (float)y;

			// mouse move callback
			MouseMoveCallback* mouse_move_callback = System::Core::getInstance().input.onMouseMove;

			// raise mouse move event
			if (mouse_move_callback) (*mouse_move_callback)(x, y);
		}

		void Input::mouseScrollCallback(GLFWwindow* window, double x, double y)
		{
			// set mouse wheel position
			System::Core::getInstance().input.z = (float)y;

			// mouse scroll callback
			MouseScrollCallback* mouse_scroll_callback = System::Core::getInstance().input.onMouseScroll;

			// raise mouse scroll event
			if (mouse_scroll_callback) (*mouse_scroll_callback)(x, y);
		}
	}
}