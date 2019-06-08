#include "transform.hpp"
#include "../objects/entity.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace hawk
{
	namespace Components
	{
		Transform::Transform() : position(0.0f), rotation(0.0f), scale(1.0f), m_world_position(0.0f), m_world_rotation(0.0f), m_world_scale(1.0f) 
		{
			// set component type
			m_type = "Transform";
		}

		Transform::Transform(const Transform& other) : position(other.position), rotation(other.rotation), scale(other.scale),
			m_world_position(other.m_world_position), m_world_rotation(other.m_world_rotation), m_world_scale(other.m_world_scale) 
		{
			// set component type
			m_type = "Transform";
		}

		Transform::Transform(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) : position(position),
			rotation(rotation), scale(scale), m_world_position(0.0f), m_world_rotation(0.0f), m_world_scale(1.0f)
		{
			// set component type
			m_type = "Transform";
		}

		Transform::~Transform() {}

		void Transform::operator=(const Transform& other)
		{
			enable = other.enable;
			position = other.position;
			rotation = other.rotation;
			scale = other.scale;
			m_world_position = other.m_world_position;
			m_world_rotation = other.m_world_rotation;
			m_world_scale = other.m_world_scale;
		}

		Transform* Transform::copy()
		{
			return new Transform(*this);
		}

		void Transform::sendMessage(const char* message)
		{
			// return if not enable
			if (!enable) return;

			// events
			if (std::strcmp(message, "Start") == 0)
				start();
			else if (std::strcmp(message, "Update") == 0)
				update();
		}

		void Transform::release()
		{
			// free object
			delete this;
		}

		void Transform::start()
		{
			// calculate vectors
			calculate();
		}

		void Transform::update()
		{
			// calculate vectors
			if (!entity->isStatic()) calculate();
		}

		void Transform::calculate(const glm::vec3& world_position, const glm::vec3& world_rotation, const glm::vec3& world_scale)
		{
			// set model matrix to identity
			m_model_matrix = glm::mat4(1.0f);

			// translate
			m_model_matrix = glm::translate(m_model_matrix, world_position + position);

			// rotate
			m_model_matrix = glm::rotate(m_model_matrix, glm::radians((world_rotation + rotation).x), glm::vec3(1.0f, 0.0f, 0.0f));
			m_model_matrix = glm::rotate(m_model_matrix, glm::radians((world_rotation + rotation).y), glm::vec3(0.0f, 1.0f, 0.0f));
			m_model_matrix = glm::rotate(m_model_matrix, glm::radians((world_rotation + rotation).z), glm::vec3(0.0f, 0.0f, 1.0f));

			// scale
			m_model_matrix = glm::scale(m_model_matrix, world_scale * scale);
		}

		glm::vec3 Transform::getWorldPosition() const
		{
			return m_world_position;
		}

		glm::vec3 Transform::getWorldRotation() const
		{
			return m_world_rotation;
		}

		glm::vec3 Transform::getWorldScale() const
		{
			return m_world_scale;
		}

		glm::mat4 Transform::getModelMatrix() const
		{
			return m_model_matrix;
		}

		void Transform::calculate()
		{
			// if entity has no parent
			if (!entity->parent)
			{
				// set world vectors to local vectors
				m_world_position = position;
				m_world_rotation = rotation;
				m_world_scale = scale;

				return;
			}

			// set initial values
			m_world_position = position;
			m_world_rotation = rotation;
			m_world_scale = scale;

			// entity parent
			const Objects::Entity* parent_entity = entity->parent;

			// traverse entity parents
			while (parent_entity)
			{
				// calculate world vectors
				m_world_position += parent_entity->transform->position;
				m_world_rotation += parent_entity->transform->rotation;
				m_world_scale *= parent_entity->transform->scale;

				// get next parent
				parent_entity = parent_entity->parent;
			}

			// set model matrix to identity
			m_model_matrix = glm::mat4(1.0f);

			// translate
			m_model_matrix = glm::translate(m_model_matrix, m_world_position);

			// rotate
			m_model_matrix = glm::rotate(m_model_matrix, glm::radians(m_world_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			m_model_matrix = glm::rotate(m_model_matrix, glm::radians(m_world_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			m_model_matrix = glm::rotate(m_model_matrix, glm::radians(m_world_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

			// scale
			m_model_matrix = glm::scale(m_model_matrix, m_world_scale);
		}
	}
}