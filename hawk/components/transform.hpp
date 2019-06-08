#ifndef HAWK_COMPONENTS_TRANSFORM_H
#define HAWK_COMPONENTS_TRANSFORM_H

// glm
#include <glm/glm.hpp>

// local
#include "component.hpp"

namespace hawk
{
	namespace Components
	{
		/**
		 * Transform component.
		 */
		class Transform : public Component
		{
		public:
			/**
			 * Constructor.
			 */
			Transform();

			/**
			 * Constructor.
			 * @param other Other object.
			 */
			Transform(const Transform& other);

			/**
			 * Constructor.
			 * @param position Transform position.
			 * @param rotation Transform rotation.
			 * @param scale Transform scale.
			 */
			Transform(const glm::vec3& position, const glm::vec3& rotation = glm::vec3(0.0f), const glm::vec3& scale = glm::vec3(1.0f));

			/**
			 * Destructor.
			 */
			~Transform();

			/**
			 * Assignment operator.
			 * @param other Other object.
			 */
			void operator=(const Transform& other);

			/**
			 * Make a copy from this object.
			 */
			Transform* copy();

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
			 * Update event.
			 */
			void update();

			/**
			 * Manually calculate vectors.
			 * @param world_position Manual world position.
			 * @param world_rotation Manual world rotation.
			 * @param world_scale Manual world scale.
			 */
			void calculate(const glm::vec3& world_position, const glm::vec3& world_rotation, const glm::vec3& world_scale);

			/**
			 * Position in world.
			 */
			glm::vec3 getWorldPosition() const;

			/**
			 * Rotation in world.
			 */
			glm::vec3 getWorldRotation() const;

			/**
			 * Scale in world.
			 */
			glm::vec3 getWorldScale() const;

			/**
			 * Model matrix in world space.
			 */
			glm::mat4 getModelMatrix() const;

			/**
			 * Local position.
			 */
			glm::vec3 position;

			/**
			 * Local rotation.
			 */
			glm::vec3 rotation;

			/**
			 * Local scale.
			 */
			glm::vec3 scale;

		private:
			/**
			 * Calculate vectors.
			 */
			void calculate();

			/**
			 * World position.
			 */
			glm::vec3 m_world_position;

			/**
			 * World rotation.
			 */
			glm::vec3 m_world_rotation;

			/**
			 * World scale.
			 */
			glm::vec3 m_world_scale;

			/**
			 * Model matrix.
			 */
			glm::mat4 m_model_matrix;
		};
	}
}
#endif