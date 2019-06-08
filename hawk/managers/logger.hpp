#ifndef HAWK_MANAGERS_LOGGER_H
#define HAWK_MANAGERS_LOGGER_H

// stl
#include <fstream>

// local
#include "manager.hpp"

namespace hawk
{
	namespace Managers
	{
		/**
		 * Log manager.
		 */
		class Logger : public Manager
		{
		public:
			/**
			 * Constructor.
			 */
			Logger();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Logger(const Logger& other) = delete;

			/**
			 * Destructor.
			 */
			~Logger();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Logger& other) = delete;

			/**
			 * Initializer.
			 * @param name Log file name. (Default name is yyyymmdd)
			 * @return True or False.
			 */
			bool initialize(const char* name = nullptr);

			/**
			 * Deinitializer.
			 */
			void deinitialize();

			/**
			 * Write info log.
			 * @param text Log text.
			 */
			void info(const char* text);

			/**
			 * Write warning log.
			 * @param text Log text.
			 */
			void warning(const char* text);

			/**
			 * Write error log.
			 * @param text Log text.
			 */
			void error(const char* text);

			/**
			 * Write log.
			 * @param source Source of generated log.
			 * @param type Type of log message.
			 * @param severity Importance of log.
			 * @param text Log message.
			 */
			void write(const char* source, const char* type, const char* severity, const char* text);

		private:
			/**
			 * Log file handle.
			 */
			std::ofstream m_file;
		};
	}
}
#endif