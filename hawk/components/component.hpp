#ifndef HAWK_COMPONENTS_COMPONENT_H
#define HAWK_COMPONENTS_COMPONENT_H

// stl
#include <string>

namespace hawk
{
	namespace Objects
	{
		/**
		 * Entity forward declaration.
		 */
		class Entity;
	}

	namespace Components
	{
		/**
		 * Components base class.
		 */
		class Component
		{
			/**
			 * Entity friend class.
			 */
			friend class hawk::Objects::Entity;

		public:
			/**
			 * Constructor.
			 */
			Component();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Component(const Component& other);

			/**
			 * Destructor.
			 */
			virtual ~Component();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Component& other);

			/**
			 * Make a copy from this object.
			 */
			virtual Component* copy();

			/**
			 * Send message to trigger events.
			 * @param message Message context.
			 */
			virtual void sendMessage(const char* message);

			/**
			 * Underlying type.
			 */
			const char* getType() const;

			/**
			 * Component id.
			 */
			unsigned int getId() const;

			/**
			 * Component name.
			 */
			const std::string& getName() const;

			/**
			 * Component availability.
			 */
			bool enable;

			/**
			 * Parent entity.
			 */
			hawk::Objects::Entity* entity;

		protected:
			/**
			 * Underlying type.
			 */
			const char* m_type;

			/**
			 * Component id.
			 */
			unsigned int m_id;

			/**
			 * Component name.
			 */
			std::string m_name;
		};
	}
}
#endif