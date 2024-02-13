#ifndef __VTX_APP_APPLICATION_SYSTEM_THREADING__
#define __VTX_APP_APPLICATION_SYSTEM_THREADING__

#include "app/application/system/system_registration.hpp"
#include "app/core/system/base_system.hpp"
#include "app/core/worker/worker_manager.hpp"

namespace VTX::App::Application::System
{
	class Threading final : public Core::System::BaseSystem, public Core::Worker::WorkerManager
	{
	  public:
		inline static const SystemRegistration<Threading> SYSTEM = SystemRegistration<Threading>();
	};
} // namespace VTX::App::Application::System

namespace VTX::App
{
	Core::Worker::WorkerManager & THREADING();
}

#endif
