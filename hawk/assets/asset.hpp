#ifndef HAWK_ASSETS_ASSET_H
#define HAWK_ASSETS_ASSET_H

// stl
#include <string>

namespace hawk
{
	namespace Managers
	{
		/**
		 * Assets forward declaration.
		 */
		class Assets;
	}

	namespace Assets
	{
		/**
		 * Asset.
		 */
		class Asset
		{
			/**
			 * Assets friend class.
			 */
			friend class hawk::Managers::Assets;

		public:
			/**
			 * Constructor.
			 */
			Asset();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Asset(const Asset& other);

			/**
			 * Destructor.
			 */
			virtual ~Asset();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Asset& other);

			/**
			 * Underlying type.
			 */
			const char* getType() const;

			/**
			 * Asset id.
			 */
			unsigned int getId() const;

			/**
			 * Asset name.
			 */
			const std::string& getName() const;

			/**
			 * Asset file path.
			 */
			const std::string& getPath() const;

			/**
			 * Asset file directory.
			 */
			const std::string& getDirectory() const;

			/**
			 * Asset safe file name.
			 */
			const std::string& getFileName() const;

			/**
			 * Asset file extension.
			 */
			const std::string& getFileExtension() const;

			/**
			 * Asset availability.
			 */
			bool enable;

		protected:
			/**
			 * Parse and set path parts.
			 */
			void setPath(const std::string& path);

			/**
			 * Underlying type.
			 */
			const char* m_type;

			/**
			 * Asset id.
			 */
			unsigned int m_id;

			/**
			 * Asset name.
			 */
			std::string m_name;

			/**
			 * Asset file path.
			 */
			std::string m_path;

			/**
			 * Asset file directory.
			 */
			std::string m_directory;

			/**
			 * Asset safe file name.
			 */
			std::string m_file_name;

			/**
			 * Asset file extension.
			 */
			std::string m_file_extension;
		};
	}
}
#endif