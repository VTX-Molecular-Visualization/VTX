#include "app/application/system/serializer.hpp"

namespace VTX::App
{
	Application::System::Serializer & SERIALIZER() { return Application::System::Serializer::SYSTEM.get(); }
} // namespace VTX::App
