#ifndef HAWK_MANAGERS_MANAGER_H
#define HAWK_MANAGERS_MANAGER_H

// stl
#include <string>

namespace hawk
{
	namespace Managers
	{
		/**
		 * Managers base class.
		 */
		class Manager
		{
		public:
			/**
			 * Constructor.
			 */
			Manager();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Manager(const Manager& other) = delete;

			/**
			 * Destructor.
			 */
			virtual ~Manager();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Manager& other) = delete;

			/**
			 * Manager name.
			 */
			const std::string& getName() const;

		protected:
			/**
			 * Manager name.
			 */
			std::string m_name;
		};
	}
}
#endif