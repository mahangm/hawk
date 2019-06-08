#include "../system/core.hpp"
#include "audio.hpp"
#include <fmod_errors.h>

namespace hawk
{
	namespace Managers
	{
		Audio::Audio() : m_studio_system(nullptr), m_low_level_system(nullptr) {}

		Audio::~Audio() {}

		bool Audio::initialize()
		{
			// application
			System::Core& app = System::Core::getInstance();

			// fmod result
			FMOD_RESULT result;

			// make studio system
			if ((result = FMOD::Studio::System::create(&m_studio_system)) != FMOD_OK)
			{
				app.logger.error("Failed to create FMOD system!");
				app.logger.info(FMOD_ErrorString(result));

				return false;
			}

			// get low level system
			if ((result = m_studio_system->getLowLevelSystem(&m_low_level_system)) != FMOD_OK)
			{
				app.logger.error("Failed to get FMOD low level system!");
				app.logger.info(FMOD_ErrorString(result));

				return false;
			}

			// set speaker setup
			if ((result = m_low_level_system->setSoftwareFormat(0, FMOD_SPEAKERMODE_DEFAULT, 0)) != FMOD_OK)
			{
				app.logger.error("Failed to set FMOD speaker setup!");
				app.logger.info(FMOD_ErrorString(result));

				return false;
			}

			// extra audio driver data
			void* extra_driver_data = nullptr;

			// initialize studio system
			if ((result = m_studio_system->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, extra_driver_data)) != FMOD_OK)
			{
				app.logger.error("Failed to initialize FMOD!");
				app.logger.info(FMOD_ErrorString(result));

				return false;
			}

			return true;
		}

		void Audio::deinitialize()
		{
			// release studio system
			if (m_studio_system)
			{
				m_studio_system->release();

				m_studio_system = nullptr;
				m_low_level_system = nullptr;
			}
		}

		void Audio::update()
		{
			// update fmod
			m_studio_system->update();
		}

		FMOD::Studio::System* Audio::getStudioSystem() const
		{
			return m_studio_system;
		}

		FMOD::System* Audio::getLowLevelSystem() const
		{
			return m_low_level_system;
		}
	}
}