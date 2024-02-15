#ifndef __VTX_APP_APPLICATION_ECS_COMPONENT_INFO__
#define __VTX_APP_APPLICATION_ECS_COMPONENT_INFO__

#include "app/application/system/system_registration.hpp"
#include "app/core/ecs/concepts.hpp"
#include "app/core/system/base_system.hpp"
#include <map>
#include <string>
#include <typeinfo>

namespace VTX::App::Application::ECS
{
	using ComponentID = std::string;

	const ComponentID INVALID_COMPONENT_ID = "";

	class ComponentInfo : public Core::System::BaseSystem
	{
	  public:
		inline static const System::SystemRegistration<ComponentInfo> SYSTEM_REG
			= System::SystemRegistration<ComponentInfo>();

	  public:
		ComponentInfo() = default;
		~ComponentInfo() {}

		template<Core::ECS::ECS_Component T>
		void registerComponent( const ComponentID & p_id )
		{
			_mapTypeID[ typeid( T ).hash_code() ] = p_id;
		}

		template<Core::ECS::ECS_Component T>
		const ComponentID & getComponentID()
		{
			const size_t hashcode = typeid( T ).hash_code();

			if ( !_mapTypeID.contains( hashcode ) )
				return INVALID_COMPONENT_ID;

			return _mapTypeID.at( hashcode );
		}

	  private:
		std::map<size_t, ComponentID> _mapTypeID = std::map<size_t, ComponentID>();
	};

} // namespace VTX::App::Application::ECS

namespace VTX::App
{
	Application::ECS::ComponentInfo & COMPONENT_INFO();
}
#endif
