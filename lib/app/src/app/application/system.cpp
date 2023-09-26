#include "app/application/system.hpp"
#include "app/application/ecs/entity_director.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/scene.hpp"
#include "app/application/setting.hpp"
#include "app/core/ecs/registry.hpp"

namespace VTX::App::Application
{
	System::System() : setting( nullptr ), registryManager( nullptr ), entityDirector( nullptr ), scene( nullptr ) {}
	System::~System() = default;
} // namespace VTX::App::Application
