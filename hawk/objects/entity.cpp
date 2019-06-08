#include "../system/core.hpp"
#include "../assets/model.hpp"
#include <algorithm>

namespace hawk
{
	namespace Objects
	{
		Entity::Entity() : onStart(nullptr), onFixedUpdate(nullptr), onUpdate(nullptr), onLateUpdate(nullptr), 
			enable(true), transform(nullptr), parent(nullptr), m_id(0), m_components_id(0), m_entities_id(0), m_is_world(false), m_is_static(false) {}

		Entity::Entity(const Entity& other) : onStart(other.onStart), onFixedUpdate(other.onFixedUpdate), onUpdate(other.onUpdate),
			onLateUpdate(other.onLateUpdate), enable(other.enable), transform(other.transform), parent(other.parent), entities(other.entities),
			m_id(other.m_id), m_name(other.m_name), m_components_id(other.m_components_id), m_entities_id(other.m_entities_id),
			m_is_world(other.m_is_world), m_is_static(other.m_is_world) {}

		Entity::Entity(bool is_world) : onStart(nullptr), onFixedUpdate(nullptr), onUpdate(nullptr), onLateUpdate(nullptr), 
			enable(true), transform(nullptr), parent(nullptr), m_id(0), m_components_id(0), m_entities_id(0), m_is_world(is_world), m_is_static(false) {}

		Entity::~Entity() {}

		void Entity::operator=(const Entity& other)
		{
			onStart = other.onStart;
			onFixedUpdate = other.onFixedUpdate;
			onUpdate = other.onUpdate;
			onLateUpdate = other.onLateUpdate;
			enable = other.enable;
			transform = other.transform;
			parent = other.parent;
			entities = other.entities;
			m_id = other.m_id;
			m_name = other.m_name;
			m_components_id = other.m_components_id;
			m_entities_id = other.m_entities_id;
			m_is_world = other.m_is_world;
			m_is_static = other.m_is_static;
		}

		Entity* Entity::copy()
		{
			return new Entity(*this);
		}

		void Entity::sendMessage(const char* message)
		{
			// return if not enable
			if (!enable) return;

			// send message to components
			for (auto component : components)
				component.second->sendMessage(message);
		}

		void Entity::release()
		{
			// release components
			for (auto component : components)
				delete component.second;

			// release entities
			for (auto entity : entities)
				entity.second->release();

			// free object
			delete this;
		}

		void Entity::start()
		{
			// return if not enable
			if (!enable) return;

			// send message to components
			for (auto component : components)
				component.second->sendMessage("Start");

			// raise start event
			if (onStart) (*onStart)(this);

			// start child entities
			for (auto entity : entities)
				entity.second->start();
		}

		void Entity::fixedUpdate()
		{
			// return if not enable
			if (!enable) return;

			// send message to components
			for (auto component : components)
				component.second->sendMessage("FixedUpdate");

			// raise fixed update event
			if (onFixedUpdate) (*onFixedUpdate)(this);

			// fixed update child entities
			for (auto entity : entities)
				entity.second->fixedUpdate();
		}

		void Entity::update()
		{
			// return if not enable
			if (!enable) return;

			// send message to components
			for (auto component : components)
				component.second->sendMessage("Update");

			// raise update event
			if (onUpdate) (*onUpdate)(this);

			// update child entities
			for (auto entity : entities)
				entity.second->update();
		}

		void Entity::lateUpdate()
		{
			// return if not enable
			if (!enable) return;

			// send message to components
			for (auto component : components)
				component.second->sendMessage("LateUpdate");

			// raise late update event
			if (onLateUpdate) (*onLateUpdate)(this);

			// late update child entities
			for (auto entity : entities)
				entity.second->lateUpdate();
		}

		void Entity::destroy()
		{
			// return if not enable
			if (!enable) return;

			// send message to components
			for (auto component : components)
				component.second->sendMessage("Destroy");

			// raise destroy event
			if (onDestroy) (*onDestroy)(this);

			// destroy child entities
			for (auto entity : entities)
				entity.second->destroy();
		}

		void Entity::removeComponent(const char* name)
		{
			// component iterator
			auto it = std::find_if(components.begin(), components.end(), [&name](std::pair<unsigned int, hawk::Components::Component*> item) {
				return (std::strcmp(item.second->getName().c_str(), name) == 0);
			});

			// if component is found
			if (it != components.end())
			{
				// reset component entity
				it->second->entity = nullptr;

				// remove from list
				components.erase(it);
			}
		}

		void Entity::clearComponents()
		{
			// clear list
			components.clear();

			// reset id
			m_components_id = 0;
		}

		Entity* Entity::addEntity(const char* name, Entity* entity)
		{
			// set entity properties
			entity->parent = this;
			entity->m_id = m_entities_id++;
			entity->m_name = name;

			// add to entities
			entities.insert(entities.end(), std::make_pair(entity->m_id, entity));

			return entity;
		}

		Entity* Entity::addEntity(const char* name, Assets::Model* model)
		{
			// new entity
			Entity* entity = instantiateSilent(model->getEntity());

			// set entity properties
			entity->parent = this;
			entity->m_id = m_entities_id++;
			entity->m_name = name;

			// add to entities
			entities.insert(entities.end(), std::make_pair(entity->m_id, entity));

			return entity;
		}

		void Entity::removeEntity(const char* name)
		{
			// entity iterator
			auto it = std::find_if(entities.begin(), entities.end(), [&name](std::pair<unsigned int, Entity*> item) {
				return (std::strcmp(item.second->getName().c_str(), name) == 0);
			});

			// if entity is found
			if (it != entities.end())
			{
				// reset entity parent
				it->second->parent = nullptr;

				// remove from list
				entities.erase(it);
			}
		}

		void Entity::clearEntities()
		{
			// clear list
			entities.clear();

			// reset id
			m_entities_id = 0;
		}

		void Entity::reset()
		{
			// reset properties
			transform = nullptr;
			parent = nullptr;

			// clear lists
			clearComponents();
			clearEntities();
		}

		Entity* Entity::instantiate(Entity* original)
		{
			// root entity
			Entity* root = original->copy();

			// reset entity
			root->reset();

			// copy components
			for (auto component : original->components)
				root->addComponent(component.second->getName().c_str(), component.second->copy());

			// copy entities
			for (auto entity : original->entities)
			{
				// child entity
				Entity* child = entity.second->copy();

				// reset child
				child->reset();

				// copy components
				for (auto component : entity.second->components)
					child->addComponent(component.second->getName().c_str(), component.second->copy());

				// add to child entities
				root->addEntity(child->getName().c_str(), child);

				// recurse copy entities
				for (auto child_child : entity.second->entities)
					child->addEntity(child_child.second->getName().c_str(), instantiate(child_child.second));
			}

			// call start event
			root->start();

			return root;
		}

		void Entity::destroy(Entity* entity)
		{
			// push to the destroyed list
			System::Core::getInstance().scene.addToDestroyedObjects(entity);
		}

		void Entity::destroyImmediate(Entity* entity)
		{
			// entity parent
			Entity* parent = entity->parent;

			// destroy entity
			entity->destroy();

			// release entity
			entity->release();

			// if entity has parent
			if (parent)
			{
				// remove from parent entities
				if (parent->entities.count(entity->getId()))
					parent->entities.erase(entity->getId());
			}
		}

		void Entity::setStatic(bool state, bool recursive)
		{
			// set self dynamic
			m_is_static = state;

			// set childeren dynamic
			for (auto entity : entities)
				entity.second->setStatic(state, recursive);
		}

		unsigned int Entity::getId() const
		{
			return m_id;
		}

		const std::string& Entity::getName() const
		{
			return m_name;
		}

		bool Entity::isWorld() const
		{
			return m_is_world;
		}

		bool Entity::isStatic() const
		{
			return m_is_static;
		}

		Entity* Entity::instantiateSilent(Entity* original)
		{
			// root entity
			Entity* root = original->copy();

			// reset entity
			root->reset();

			// copy components
			for (auto component : original->components)
				root->addComponent(component.second->getName().c_str(), component.second->copy());

			// copy entities
			for (auto entity : original->entities)
			{
				// child entity
				Entity* child = entity.second->copy();

				// reset child
				child->reset();

				// copy components
				for (auto component : entity.second->components)
					child->addComponent(component.second->getName().c_str(), component.second->copy());

				// add to child entities
				root->addEntity(child->getName().c_str(), child);

				// recurse copy entities
				for (auto child_child : entity.second->entities)
					child->addEntity(child_child.second->getName().c_str(), instantiate(child_child.second));
			}

			return root;
		}
	}
}