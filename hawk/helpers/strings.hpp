#ifndef HAWK_HELPERS_STRINGS_H
#define HAWK_HELPERS_STRINGS_H

// stl
#include <vector>
#include <string>
#include <locale>

namespace hawk
{
	namespace Helpers
	{
		namespace Strings
		{
			/**
			 * Split a string.
			 * @param source Source string.
			 * @param delimiter Delimiter char.
			 * @return A vector of splitted strings.
			 */
			static std::vector<std::string> tokenize(const char* source, const char* delimiter)
			{
				// source string length
				size_t source_len = strlen(source);

				// source string copy
				char* source_copy = new char[source_len + 1];

				// copy source string
				memcpy(source_copy, source, source_len);

				source_copy[source_len] = 0;

				// next token
				char* next_token = nullptr;

				// current token
				char* token = strtok_s(source_copy, delimiter, &next_token);

				// tokens
				std::vector<std::string> tokens;

				// when there is a token
				while (token)
				{
					// add token
					tokens.push_back(token);

					// next token
					token = strtok_s(nullptr, delimiter, &next_token);
				}

				// free source string copy
				delete[] source_copy;

				return tokens;
			}

			/**
			 * Replace a string.
			 * @param source Source string.
			 * @param search Search string.
			 * @param replace Replace string.
			 * @return True or False.
			 */
			static bool replace(std::string& source, const std::string& search, const std::string& replace)
			{
				// search string position
				size_t pos = source.find(search);

				// return if not found
				if (pos == std::string::npos) return false;

				// replace string
				source.replace(pos, search.length(), replace);

				return true;
			}

			/**
			 * Convert a string to lower case.
			 * @param source Source string.
			 * @return Lower case string.
			 */
			static std::string lowercase(const std::string& source)
			{
				// locale
				std::locale locale;

				// result string
				std::string result;

				// convert
				for (std::string::size_type i = 0; i < source.length(); ++i)
					result += std::tolower(source[i], locale);

				return result;
			}

			/**
			 * Convert a string to upper case.
			 * @param source Source string.
			 * @return Upper case string.
			 */
			static std::string uppercase(const std::string& source)
			{
				// locale
				std::locale locale;

				// result string
				std::string result;

				// convert
				for (std::string::size_type i = 0; i < source.length(); ++i)
					result += std::toupper(source[i], locale);

				return result;
			}
		}
	}
}
#endif