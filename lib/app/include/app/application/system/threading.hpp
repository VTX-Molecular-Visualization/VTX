#ifndef __VTX_APP_APPLICATION_SYSTEM_THREADING__
#define __VTX_APP_APPLICATION_SYSTEM_THREADING__

#include "app/core/worker/worker_manager.hpp"

namespace VTX::App::Application::System
{
	class Threading final : public Core::Worker::WorkerManager
	{
	};
} // namespace VTX::App::Application::System

namespace VTX::App
{
	// Access to the worker manager class in order to launch thread.
	inline Core::Worker::WorkerManager & THREADING() { return Util::Singleton<Application::System::Threading>::get(); }
} // namespace VTX::App

#endif
