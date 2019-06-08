#ifndef HAWK_OBJECTS_ENTITY_H
#define HAWK_OBJECTS_ENTITY_H

// stl
#include <map>

// local
#include "../components/component.hpp"
#include "../components/transform.hpp"

namespace hawk
{
	namespace Assets
	{
		/**
		 * Model forward reference.
		 */
		class Model;
	}

	namespace Objects
	{
		/**
		 * Enttiy.
		 */
		class Entity
		{
		public:
			/**
			 * Constructor.
			 */
			Entity();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Entity(const Entity& other);

			/**
			 * Constructor.
			 * @param is_world True if this is world entity.
			 */
			Entity(bool is_world);

			/**
			 * Destructor.
			 */
			~Entity();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			virtual void operator=(const Entity& other);

			/**
			 * Make a copy from this object.
			 */
			Entity* copy();

			/**
			 * Send message to trigger events.
			 * @param message Message context.
			 */
			void sendMessage(const char* message);

			/**
			 * Release resources.
			 */
			void release();

			/**
			 * Start event.
			 */
			void start();

			/**
			 * Fixed update event.
			 */
			void fixedUpdate();

			/**
			 * Update event.
			 */
			void update();

			/**
			 * Late update event.
			 */
			void lateUpdate();

			/**
			 * Destroy event.
			 */
			void destroy();

			/**
			 * Add a component to entity's components.
			 * @param name Component name.
			 * @param component Component object.
			 * @return A pointer to component.
			 */
			template<typename T>
			T* addComponent(const char* name, T* component);

			/**
			 * Remove a component from entity's components.
			 * @param name Component name.
			 */
			void removeComponent(const char* name);

			/**
			 * Clear components.
			 */
			void clearComponents();

			/**
			 * Add an entity to child entities.
			 * @param name Child name.
			 * @param entity Child entity.
			 * @return A pointer to child entity.
			 */
			Entity* addEntity(const char* name, Entity* entity);

			/**
			 * Add a model to child entities.
			 * @param name Child name.
			 * @param model Model asset.
			 * @return A pointer to child entity.
			 */
			Entity* addEntity(const char* name, Assets::Model* model);

			/**
			 * Remove an entity from child entities.
			 * @param name Child name.
			 */
			void removeEntity(const char* name);

			/**
			 * Clear entities.
			 */
			void clearEntities();

			/**
			 * Reset entity for instantiation.
			 */
			void reset();

			/**
			 * Instantiate an entity.
			 * @param original Original entity to be copied from.
			 * @return A pointer to newly made entity.
			 */
			static Entity* instantiate(Entity* original);

			/**
			 * Destroy an entity on end of frame.
			 * @param entity Entity to be destroyed.
			 */
			static void destroy(Entity* entity);

			/**
			 * Destroy an entity immediately.
			 * @param entity Entity to be destroyed.
			 */
			static void destroyImmediate(Entity* entity);

			/**
			 * Set entity and its childeren movement dynamic.
			 * @param state True or False.
			 * @param recursive Set child entities dynamic as well.
			 */
			void setStatic(bool state, bool recursive = false);

			/**
			 * Entity id.
			 */
			unsigned int getId() const;

			/**
			 * Entity name.
			 */
			const std::string& getName() const;

			/**
			 * Is this world entity?
			 */
			bool isWorld() const;

			/**
			 * Does entity move?
			 */
			bool isStatic() const;

			/**
			 * Start callback type.
			 */
			typedef void(StartCallback)(Entity* entity);

			/**
			 * Fixed update callback type.
			 */
			typedef void(FixedUpdateCallback)(Entity*);

			/**
			 * Update callback type.
			 */
			typedef void(UpdateCallback)(Entity*);

			/**
			 * Late update callback type.
			 */
			typedef void(LateUpdateCallback)(Entity*);

			/**
			 * Destroy callback type.
			 */
			typedef void(DestroyCallback)(Entity*);

			/**
			 * Start event.
			 */
			StartCallback* onStart;

			/**
			 * Fixed update event.
			 */
			FixedUpdateCallback* onFixedUpdate;

			/**
			 * Update event.
			 */
			UpdateCallback* onUpdate;

			/**
			 * Late update event.
			 */
			LateUpdateCallback* onLateUpdate;

			/**
			 * Destroy event.
			 */
			DestroyCallback* onDestroy;

			/**
			 * Availability of entity.
			 */
			bool enable;

			/**
			 * Entity transform.
			 */
			Components::Transform* transform;

			/**
			 * List of entity components.
			 */
			std::map<unsigned int, Components::Component*> components;

			/**
			 * Parent entity.
			 */
			Entity* parent;

			/**
			 * List of child entities.
			 */
			std::map<unsigned int, Entity*> entities;

		private:
			/**
			 * Instantiate an entity without calling initial events like start.
			 * @param original Original entity to be copied from.
			 * @return A pointer to newly made entity.
			 */
			Entity* instantiateSilent(Entity* original);

			/**
			 * Entity id.
			 */
			unsigned int m_id;

			/**
			 * Entity name.
			 */
			std::string m_name;

			/**
			 * Components id holder.
			 */
			unsigned int m_components_id;

			/**
			 * Entities id holder.
			 */
			unsigned int m_entities_id;

			/**
			 * Is this world entity?
			 */
			bool m_is_world;

			/**
			 * Does entity move?
			 */
			bool m_is_static;
		};

		template<typename T>
		T* Entity::addComponent(const char* name, T* component)
		{
			// set component properties
			component->entity = this;
			component->m_id = m_components_id++;
			component->m_name = name;

			// add to components
			components.insert(components.end(), std::make_pair(component->m_id, component));

			// set transform component
			if (std::strcmp(component->getType(), "Transform") == 0)
				transform = dynamic_cast<Components::Transform*>(component);

			return component;
		}
	}
}
#endif