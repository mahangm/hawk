#ifndef HAWK_HELPERS_FILE_SYSTEM_H
#define HAWK_HELPERS_FILE_SYSTEM_H

// stl
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

// local
#include "strings.hpp"
#include "memory.hpp"

namespace hawk
{
	namespace Helpers
	{
		namespace FileSystem
		{
			/**
			 * Check directory existence.
			 * @param path Directory path.
			 * @return True or False.
			 */
			static bool directoryExists(const char* path)
			{
				struct stat info;

				// check stat
				if (stat(path, &info) != 0)
					return false;
				else if (info.st_mode & S_IFDIR)
					return true;
				else
					return false;
			}

			/**
			 * Make directory recursively.
			 * @param path Directory path.
			 * @return True or False.
			 */
			static bool makeDirectory(const char* path)
			{
				// path parts
				std::vector<std::string> parts = hawk::Helpers::Strings::tokenize(path, "/");

				// directory path holder
				std::string directory_path = "";

				// iterate parts
				for (auto& part : parts)
				{
					// make path
					directory_path += part + "/";

					// if directory path does not exist make path
					if (!directoryExists(directory_path.c_str()))
						if (_mkdir(directory_path.c_str()) != 0) return false;
				}

				// make directory
				if (_mkdir(path) != 0) return false;

				return true;
			}

			/**
			 * Check file existence.
			 * @param path File path.
			 * @return True or False.
			 */
			static bool fileExists(const char* path)
			{
				// file info
				struct stat info;

				// check stat
				if (stat(path, &info) != 0)
					return false;
				else if (info.st_mode & S_IFDIR)
					return false;
				else
					return true;
			}

			/**
			 * Read text file.
			 * @param path File path.
			 * @param buffer File content.
			 * @return True or False.
			 */
			static bool readTextFile(const char* path, std::string& buffer)
			{
				// file stream
				std::ifstream file(path, std::ios::in);

				// if failed to open file return
				if (!file.is_open()) return false;

				// string file content
				std::stringstream stream;

				// read file content
				stream << file.rdbuf();

				// close file
				file.close();

				// set file content
				buffer = stream.str();

				return true;
			}

			/**
			 * Read binary file.
			 * @param path File path.
			 * @param buffer File content.
			 * @return True or False.
			 */
			static bool readBinaryFile(const char* path, char** buffer)
			{
				// file stream
				std::ifstream file(path, std::ios::in | std::ios::binary);

				// if failed to open file return
				if (!file.is_open()) return false;

				// file length
				size_t length = 0;

				// get length
				file.seekg(0, std::ios::end);

				length = file.tellg();

				file.seekg(0, std::ios::beg);

				// make buffer
				*buffer = new char[length];

				// zeroize buffer
				hawk::Helpers::Memory::zeroize(*buffer, sizeof *buffer);

				// read content
				std::streambuf* raw_buffer = file.rdbuf();

				raw_buffer->sgetn(*buffer, length);

				// close file
				file.close();

				return true;
			}

			/**
			 * Get file directory from file path.
			 * @param path Relative or full file path.
			 * @return Directory with trailing slash.
			 */
			static std::string getDirectory(const std::string& path)
			{
				// result
				std::string result = path;

				// replace back slash with forward slash
				std::replace(result.begin(), result.end(), '\\', '/');

				return result.substr(0, result.find_last_of('/') + 1);
			}

			/**
			 * Get file name from file path.
			 * @param path Relative or full file path.
			 * @return File name with extension.
			 */
			static std::string getFileName(const std::string& path)
			{
				// result
				std::string result = path;

				// replace back slash with forward slash
				std::replace(result.begin(), result.end(), '\\', '/');

				return result.substr(result.find_last_of('/') + 1);
			}

			/**
			 * Get file extension from file name.
			 * @param file_name File name with extension.
			 * @return Extension string without dot.
			 */
			static std::string getExtension(const std::string& file_name)
			{
				// last dot position
				std::size_t last_dot = file_name.find_last_of(".");

				return (last_dot != std::string::npos) ? file_name.substr(last_dot + 1, file_name.length() - (last_dot + 1)) : std::string();
			}
		}
	}
}
#endif