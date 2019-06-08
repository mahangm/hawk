#include "logger.hpp"
#include "../helpers/file_system.hpp"
#include <Windows.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdio>

namespace hawk
{
	namespace Managers
	{
		Logger::Logger()
		{
			// set manager name
			m_name = "Logger";
		}

		Logger::~Logger() {}

		bool Logger::initialize(const char* name)
		{
			// logs directory
			const char* logs_dir = "../logs/";

			// make logs directory if not exists
			if (!hawk::Helpers::FileSystem::directoryExists(logs_dir))
				if (!hawk::Helpers::FileSystem::makeDirectory(logs_dir)) return false;

			// log file name
			char log_name[MAX_PATH] = { 0 };

			// make log name
			if (name)
			{
				std::snprintf(log_name, sizeof log_name, "%s", name);
			}
			else
			{
				// get system time
				SYSTEMTIME system_time;

				GetSystemTime(&system_time);

				std::snprintf(log_name, sizeof log_name, "%.4d%.2d%.2d", system_time.wYear, system_time.wMonth, system_time.wDay);
			}

			// open log file
			m_file.open(std::string(logs_dir).append(log_name).append(".log").c_str(), std::ios::app);

			// check file
			if (!m_file.is_open()) return false;

			return true;
		}

		void Logger::deinitialize()
		{
			// close log file
			if (m_file.is_open()) m_file.close();
		}

		void Logger::info(const char* text)
		{
			write("Application", "Info", "Low", text);
		}

		void Logger::warning(const char* text)
		{
			write("Application", "Warning", "Medium", text);
		}

		void Logger::error(const char* text)
		{
			write("Application", "Error", "High", text);
		}

		void Logger::write(const char* source, const char* type, const char* severity, const char* text)
		{
			// get system time
			SYSTEMTIME system_time;

			GetSystemTime(&system_time);

			// log text
			std::stringstream output;

			// left align text
			output << std::left;

			// log date in format of YYYY-MM-DD H:i:s
			char log_date[32] = { 0 };

			std::snprintf(log_date, sizeof log_date, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d", system_time.wYear, system_time.wMonth, system_time.wDay, system_time.wHour, system_time.wMinute, system_time.wSecond);

			// write log time
			output << std::setw(32) << log_date;

			// write log source
			output << std::setw(24) << source;

			// write log type
			output << std::setw(24) << type;

			// write log severity
			output << std::setw(16) << severity;

			// write log text
			output << text << std::endl;

			// make string
			std::string output_str = output.str();

			// write to log file
			m_file.write(output_str.c_str(), output_str.length());

			// update log file content
			m_file.flush();

			// write to standard output
			std::cout << text << std::endl;
		}
	}
}