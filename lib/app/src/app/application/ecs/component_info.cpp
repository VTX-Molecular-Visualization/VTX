#include "app/application/ecs/component_info.hpp"

namespace VTX::App::Application::ECS
{
} // namespace VTX::App::Application::ECS

namespace VTX::App
{
	Application::ECS::ComponentInfo & COMPONENT_INFO() { return Application::ECS::ComponentInfo::SYSTEM_REG.get(); }
} // namespace VTX::App
