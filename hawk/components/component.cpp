#include "../system/core.hpp"
#include "component.hpp"

namespace hawk
{
	namespace Components
	{
		Component::Component() : enable(true), m_type("Component"), m_id(0), entity(nullptr) {}

		Component::Component(const Component& other) : enable(other.enable), m_type(other.m_type), m_id(other.m_id), 
			m_name(other.m_name), entity(other.entity) {}

		Component::~Component() {}

		void Component::operator=(const Component& other)
		{
			enable = other.enable;
			m_type = other.m_type;
			m_id = other.m_id;
			m_name = other.m_name;
			entity = other.entity;
		}

		Component* Component::copy()
		{
			return new Component(*this);
		}

		void Component::sendMessage(const char* message)
		{
			// return if not enable
			if (!enable) return;
		}

		const char* Component::getType() const
		{
			return m_type;
		}

		unsigned int Component::getId() const
		{
			return m_id;
		}

		const std::string& Component::getName() const
		{
			return m_name;
		}
	}
}