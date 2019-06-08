#include "../system/core.hpp"
#include "asset.hpp"
#include "../helpers/strings.hpp"
#include "../helpers/file_system.hpp"

namespace hawk
{
	namespace Assets
	{
		Asset::Asset() : enable(true), m_type("Asset"), m_id(0) {}

		Asset::Asset(const Asset& other) : enable(other.enable), m_type(other.m_type), m_id(other.m_id), m_name(other.m_name), 
			m_path(other.m_path), m_directory(other.m_directory), m_file_name(other.m_file_name), m_file_extension(other.m_file_extension) {}

		Asset::~Asset() {}

		void Asset::operator=(const Asset& other)
		{
			enable = other.enable;
			m_type = other.m_type;
			m_id = other.m_id;
			m_name = other.m_name;
			m_path = other.m_path;
			m_directory = other.m_directory;
			m_file_name = other.m_file_name;
			m_file_extension = other.m_file_extension;
		}

		const char* Asset::getType() const
		{
			return m_type;
		}

		unsigned int Asset::getId() const
		{
			return m_id;
		}

		const std::string& Asset::getName() const
		{
			return m_name;
		}

		const std::string& Asset::getPath() const
		{
			return m_path;
		}

		const std::string& Asset::getDirectory() const
		{
			return m_directory;
		}

		const std::string& Asset::getFileName() const
		{
			return m_file_name;
		}

		const std::string& Asset::getFileExtension() const
		{
			return m_file_extension;
		}

		void Asset::setPath(const std::string& path)
		{
			// set file path
			m_path = path;

			// set file directory
			m_directory = Helpers::FileSystem::getDirectory(path);

			// set file name
			m_file_name = Helpers::FileSystem::getFileName(path);

			// set file extension
			m_file_extension = Helpers::Strings::lowercase(Helpers::FileSystem::getExtension(m_file_name));

			// remove extension from file name
			m_file_name = m_file_name.substr(0, m_file_name.find_last_of("."));
		}
	}
}