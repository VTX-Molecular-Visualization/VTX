#ifndef __VTX_APP_APPLICATION_SYSTEM_THREADING__
#define __VTX_APP_APPLICATION_SYSTEM_THREADING__

#include "app/application/system/system_registration.hpp"
#include "app/core/system/base_system.hpp"
#include "app/core/worker/worker_manager.hpp"

namespace VTX::App::Application::System
{
	class Threading final : public System::AutoRegistrateSystem<Threading>, public Core::Worker::WorkerManager
	{
	};
} // namespace VTX::App::Application::System

namespace VTX::App
{
	Core::Worker::WorkerManager & THREADING();
}

#endif
