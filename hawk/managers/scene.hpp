#ifndef HAWK_MANAGERS_SCENE_H
#define HAWK_MANAGERS_SCENE_H

// stl
#include <map>

// local
#include "manager.hpp"
#include "../objects/entity.hpp"

namespace hawk
{
	namespace Managers
	{
		/**
		 * Scene manager.
		 */
		class Scene : public Manager
		{
		public:
			/**
			 * Constructor.
			 */
			Scene();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Scene(const Scene& other) = delete;

			/**
			 * Destructor.
			 */
			~Scene();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Scene& other) = delete;

			/**
			 * Initializer.
			 * @return True or False.
			 */
			bool initialize();

			/**
			 * Deinitialzer.
			 */
			void deinitialize();

			/**
			 * Release scene.
			 */
			void release();

			/**
			 * Add entity to be destroyed on end of frame.
			 * @param entity Entity to be destroyed.
			 */
			void addToDestroyedObjects(Objects::Entity* entity);

			/**
			 * Handle destroyed objects.
			 */
			void processDestroyedObjects();

			/**
			 * World entity.
			 */
			Objects::Entity* world;

		private:
			/**
			 * Destroyed objects list.
			 */
			std::map<const char*, Objects::Entity*> m_destroyed_objects;
		};
	}
}
#endif