#ifndef HAWK_SYSTEM_CORE_H
#define HAWK_SYSTEM_CORE_H

#if defined(_WIN32) || defined(WIN32)
// windows identifier
#define WINDOWS
#elif defined(__unix__)
// linux identifier
#define LINUX
#elif defined(__APPLE__)
// mac identifier
#define MAC
#endif

// glad
#include <glad/glad.h>

// glfw
#include <GLFW/glfw3.h>

// glm
#include <glm/glm.hpp>

// local
#include "../managers/logger.hpp"
#include "../managers/time.hpp"
#include "../managers/audio.hpp"
#include "../managers/video.hpp"
#include "../managers/input.hpp"
#include "../managers/scene.hpp"
#include "../managers/assets.hpp"

namespace hawk
{
	namespace System
	{
		/**
		 * Hawk's core.
		 */
		class Core
		{
		public:
			/**
			 * Sole core instance.
			 */
			static Core& getInstance();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Core(const Core& other) = delete;

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Core& other) = delete;

			/**
			 * Initializer.
			 * @param width Window width.
			 * @param height Window height.
			 * @param title Window title.
			 * @param rendering Rendering path.
			 * @param lighting Lighting method.
			 * @param max_lights Total lights count.
			 * @param max_shadows Total shadows count.
			 * @param log_path Log file path.
			 */
			bool initialize(int width, int height, const char* title, 
				Managers::Video::Rendering rendering = Managers::Video::Rendering::Forward, 
				Managers::Video::Lighting lighting = Managers::Video::Lighting::BlinnPhong,
				unsigned int max_lights = 4, unsigned int max_shadows = 4);

			/**
			 * Deinitializer.
			 */
			void deinitialize();

			/**
			 * Start world.
			 */
			void start();

			/**
			 * Update world with a fixed timer.
			 */
			void fixedUpdate();

			/**
			 * Update world.
			 */
			void update();

			/**
			 * Updated world operations.
			 */
			void lateUpdate();

			/**
			 * Render world.
			 */
			void render();

			/**
			 * Destroy candidate objects.
			 */
			void destroy();

			/**
			 * Stop core.
			 */
			void stop();

			/**
			 * Swap buffers and process events.
			 */
			void process();

			/**
			 * Is core running?
			 * @return True or False.
			 */
			bool isRunning() const;

			/**
			 * Window resize callback type.
			 */
			typedef void(WindowResizeCallback)(int width, int height);

			/**
			 * Window resize event.
			 */
			WindowResizeCallback* onWindowResize;

			/**
			 * Log manager.
			 */
			Managers::Logger logger;

			/**
			 * Time manager.
			 */
			Managers::Time time;

			/**
			 * Input manager.
			 */
			Managers::Input input;

			/**
			 * Audio manager.
			 */
			Managers::Audio audio;

			/**
			 * Graphics manager.
			 */
			Managers::Video video;

			/**
			 * Asset manager.
			 */
			Managers::Assets assets;

			/**
			 * Scene manager.
			 */
			Managers::Scene scene;

			/**
			 * Main window.
			 */
			GLFWwindow* window;

			/**
			 * Main window width.
			 */
			int width;

			/**
			 * Main window height.
			 */
			int height;

			/**
			 * Main window iconification.
			 */
			bool iconified;

		private:
			/**
			 * Constructor.
			 */
			Core();

			/**
			 * Destructor.
			 */
			~Core();

			/**
			 * GFLW error callback.
			 * @param code Error code.
			 * @param message Error message.
			 */
			static void glfwErrorCallback(int code, const char* message);

			/**
			 * Frame buffer size callback.
			 * @param window GLFW window.
			 * @param width New width.
			 * @param height New height.
			 */
			static void windowResizeCallback(GLFWwindow* window, int width, int height);

			/**
			 * Window iconification callback.
			 * @param window GLFW window.
			 * @param iconified Iconification result.
			 */
			static void windowIconifyCallback(GLFWwindow* window, int iconified);

			/**
			 * OpenGL debug output callback.
			 */
			static void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, 
				GLsizei length, const GLchar* message, const void* user_param);

			/**
			 * Binary directory.
			 */
			char m_bin_dir[MAX_PATH];

			/**
			 * Application directory.
			 */
			char m_app_dir[MAX_PATH];

			/**
			 * Running flag.
			 */
			bool m_running;
		};
	}
}
#endif