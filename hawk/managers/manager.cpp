#include "../system/core.hpp"
#include "manager.hpp"

namespace hawk
{
	namespace Managers
	{
		Manager::Manager() : m_name("Manager") {}

		Manager::~Manager() {}

		const std::string& Manager::getName() const
		{
			return m_name;
		}
	}
}