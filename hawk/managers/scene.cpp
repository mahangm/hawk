#include "scene.hpp"

namespace hawk
{
	namespace Managers
	{
		Scene::Scene() : world(nullptr)
		{
			// set manager name
			m_name = "Scene";
		}

		Scene::~Scene() {}

		bool Scene::initialize()
		{
			// make world
			world = new Objects::Entity(true);
			world->addComponent("Transform", new Components::Transform());

			return true;
		}

		void Scene::deinitialize()
		{
			// free resources
			release();
		}

		void Scene::release()
		{
			// release world
			if (world)
			{
				world->release();

				world = nullptr;
			}
		}

		void Scene::addToDestroyedObjects(Objects::Entity* entity)
		{
			m_destroyed_objects.insert(m_destroyed_objects.end(), std::make_pair(entity->getName().c_str(), entity));
		}

		void Scene::processDestroyedObjects()
		{
			// destroy collected objects
			for (auto entity : m_destroyed_objects)
				Objects::Entity::destroyImmediate(entity.second);

			// clear list
			m_destroyed_objects.clear();
		}
	}
}