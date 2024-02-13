#include "app/application/system/threading.hpp"

namespace VTX::App
{
	Core::Worker::WorkerManager & THREADING() { return Application::System::Threading::SYSTEM.get(); }
} // namespace VTX::App
