#include "core.hpp"
#include "../helpers/memory.hpp"
#include <objbase.h>

namespace hawk
{
	namespace System
	{
		Core& Core::getInstance()
		{
			// sole core instance
			static Core instance;

			return instance;
		}

		Core::Core() : onWindowResize(nullptr), window(nullptr), width(0), height(0), iconified(false), m_running(false)
		{
			// zeroize
			hawk::Helpers::Memory::zeroize(m_bin_dir, sizeof m_bin_dir);
			hawk::Helpers::Memory::zeroize(m_app_dir, sizeof m_app_dir);
		}

		Core::~Core() {}

		bool Core::initialize(int width, int height, const char* title, 
			Managers::Video::Rendering rendering, 
			Managers::Video::Lighting lighting, 
			unsigned int max_lights, unsigned int max_shadows)
		{
			// initialize com library for fmod
			CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

			// get bin directory
			GetModuleFileName(GetModuleHandle(nullptr), m_bin_dir, sizeof m_bin_dir);

			m_bin_dir[std::strrchr(m_bin_dir, '\\') - m_bin_dir] = 0;

			// get app directory
			hawk::Helpers::Memory::copy(m_bin_dir, m_app_dir, sizeof m_app_dir);

			m_app_dir[std::strrchr(m_app_dir, '\\') - m_app_dir] = 0;

			// set current directory
			if (!SetCurrentDirectory(m_bin_dir))
			{
				printf("Failed to set current directory!\n");

				return 1;
			}

			// initialize logger
			if (!logger.initialize())
			{
				printf("Failed to initialize logger!\n");

				return false;
			}

			logger.info("Initializing core...");

			logger.info("Initializing GLFW...");

			// initialize glfw
			if (glfwInit() != GLFW_TRUE)
			{
				logger.error("Failed to initialize GLFW!");

				return false;
			}

			// set opengl version
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GL_TRUE);
#ifdef MAC
			// set opengl forward compatiblity
			glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
#ifdef DEBUG
			// set debug context
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
#endif
			// create main window
			window = glfwCreateWindow(width, height, title, nullptr, nullptr);

			// check window
			if (!window)
			{
				logger.error("Failed to initialize GLFW!");
				logger.error("Failed to create main window!");

				// terminate glfw
				glfwTerminate();

				return false;
			}

			// set callbacks
			glfwSetErrorCallback(Core::glfwErrorCallback);
			glfwSetFramebufferSizeCallback(window, Core::windowResizeCallback);
			glfwSetWindowIconifyCallback(window, Core::windowIconifyCallback);

			// make opengl context
			glfwMakeContextCurrent(window);

			logger.info("Initializing GLAD...");

			// initialize glad
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				logger.error("Failed to initialize GLAD!");

				// terminate glfw
				glfwTerminate();

				return false;
			}
#ifdef DEBUG
			// opengl context flags
			int context_falgs = 0;

			// get opengl context flags
			glGetIntegerv(GL_CONTEXT_FLAGS, &context_falgs);

			// check debug flag
			if (context_falgs & GL_CONTEXT_FLAG_DEBUG_BIT)
			{
				// enable debug output
				glEnable(GL_DEBUG_OUTPUT);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				
				// set callback
				glDebugMessageCallback(glDebugOutput, nullptr);

				// filter output
				glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
			}
#endif
			logger.info("Initializing input...");

			// initialize input
			if (!input.initialize(window))
			{
				logger.error("Failed to initialize input!");

				// terminate glfw
				glfwTerminate();

				return false;
			}

			logger.info("Initializing audio...");

			// initialize audio
			if (!audio.initialize())
			{
				logger.error("Failed to initialize audio!");

				// deinitialize
				audio.deinitialize();

				// terminate glfw
				glfwTerminate();

				return false;
			}

			logger.info("Initializing video...");

			// initialize video
			if (!video.initialize(width, height, rendering, lighting, max_lights, max_shadows))
			{
				logger.error("Failed to initialize video!");

				// deinitialize
				video.deinitialize();

				// terminate glfw
				glfwTerminate();

				return false;
			}

			logger.info("Initializing assets...");

			// initialize assets
			if (!assets.initialize())
			{
				logger.error("Failed to initialize assets!");

				// deinitialize
				assets.deinitialize();

				// terminate glfw
				glfwTerminate();

				return false;
			}

			logger.info("Initializing scene...");

			// initialize scene
			if (!scene.initialize())
			{
				logger.error("Failed to initialize scene!");

				// deinitialize
				scene.deinitialize();

				// terminate glfw
				glfwTerminate();

				return false;
			}

			// center window
			glfwSetWindowPos(window, (video.getVideoMode()->width - width) / 2, (video.getVideoMode()->height - height) / 2);

			logger.info("Initialized core successfully.");

			// set running flag
			m_running = true;

			// set window size
			this->width = width;
			this->height = height;

			return true;
		}

		void Core::deinitialize()
		{
			logger.info("Deinitializing core...");

			// deinit scene
			scene.deinitialize();

			// deinit assets
			assets.deinitialize();

			// deinit video
			video.deinitialize();

			// deinit audio
			audio.deinitialize();

			logger.info("Deinitialized core successfully.");

			// deinitialize logger
			logger.deinitialize();

			// close main window
			glfwSetWindowShouldClose(window, GL_TRUE);

			// terminate glfw
			glfwTerminate();

			// deinitialize com library
			CoUninitialize();
		}

		void Core::start()
		{
			// start time manager
			time.start();

			// start world
			scene.world->start();

			// start video manager
			video.start();
		}

		void Core::fixedUpdate()
		{
			// loop until we have enough frames to do fixed update
			while (time.hasFixedFrames())
			{
				// update time manager
				time.fixedUpdate();

				// update audio manager
				audio.update();

				// update world with fixed frame rate
				scene.world->fixedUpdate();
			}
		}

		void Core::update()
		{
			// stop core if main window should close
			if (glfwWindowShouldClose(window))
				m_running = false;

			// update time manager
			time.update();

			// update input manager
			input.update();

			// update world
			scene.world->update();
		}

		void Core::lateUpdate()
		{
			// late update world
			scene.world->lateUpdate();
		}

		void Core::render()
		{
			// return if window is iconified
			if (iconified) return;

			// render scene to cameras
			video.render();

			// render main camera to back buffer
			video.postProcess();
		}

		void Core::destroy()
		{
			// process destroyed objects
			scene.processDestroyedObjects();
		}

		void Core::stop()
		{
			// set running flag
			m_running = false;
		}

		void Core::process()
		{
			// swap back buffer to front buffer
			glfwSwapBuffers(window);

			// process events
			glfwPollEvents();
		}

		bool Core::isRunning() const
		{
			// get running flag
			return m_running;
		}

		void Core::glfwErrorCallback(int code, const char* message)
		{
			// write log
			System::Core::getInstance().logger.error(message);
		}

		void Core::windowResizeCallback(GLFWwindow* window, int width, int height)
		{
			// reset viewport
			glViewport(0, 0, width, height);

			// set window size
			System::Core::getInstance().width = width;
			System::Core::getInstance().height = height;

			// window resize callback
			WindowResizeCallback* window_resize_callback = System::Core::getInstance().onWindowResize;

			// raise window resize event
			if (window_resize_callback) (*window_resize_callback)(width, height);
		}

		void Core::windowIconifyCallback(GLFWwindow* window, int iconified)
		{
			// set window iconification
			System::Core::getInstance().iconified = (iconified > 0);
		}

		void APIENTRY Core::glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, 
			GLsizei length, const GLchar* message, const void* user_param)
		{
			// ignore non-significant error/warning codes
			if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

			// app
			Core& app = Core::getInstance();

			// error source string
			const char* source_str = "";

			// error type string
			const char* type_str = "";

			// error severity string
			const char* severity_str = "";

			// get error source
			switch (source)
			{
			case GL_DEBUG_SOURCE_API:             source_str = "API"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_str = "Window System"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: source_str = "Shader Compiler"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:     source_str = "Third Party"; break;
			case GL_DEBUG_SOURCE_APPLICATION:     source_str = "Application"; break;
			case GL_DEBUG_SOURCE_OTHER:           source_str = "Other"; break;
			}

			// get error type
			switch (type)
			{
			case GL_DEBUG_TYPE_ERROR:               type_str = "Error"; break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "Deprecated Behaviour"; break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_str = "Undefined Behaviour"; break;
			case GL_DEBUG_TYPE_PORTABILITY:         type_str = "Portability"; break;
			case GL_DEBUG_TYPE_PERFORMANCE:         type_str = "Performance"; break;
			case GL_DEBUG_TYPE_MARKER:              type_str = "Marker"; break;
			case GL_DEBUG_TYPE_PUSH_GROUP:          type_str = "Push Group"; break;
			case GL_DEBUG_TYPE_POP_GROUP:           type_str = "Pop Group"; break;
			case GL_DEBUG_TYPE_OTHER:               type_str = "Other"; break;
			}

			// get error severity
			switch (severity)
			{
			case GL_DEBUG_SEVERITY_HIGH:         severity_str = "High"; break;
			case GL_DEBUG_SEVERITY_MEDIUM:       severity_str = "Medium"; break;
			case GL_DEBUG_SEVERITY_LOW:          severity_str = "Low"; break;
			case GL_DEBUG_SEVERITY_NOTIFICATION: severity_str = "Notification"; break;
			}

			// log message
			app.logger.write(source_str, type_str, severity_str, message);
		}
	}
}