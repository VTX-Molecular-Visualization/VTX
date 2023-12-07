#ifndef __VTX_APP_APPLICATION_ECS_COMPONENT_INFO__
#define __VTX_APP_APPLICATION_ECS_COMPONENT_INFO__

#include "app/core/ecs/base_entity.hpp"
#include "app/core/ecs/concepts.hpp"
#include <map>
#include <string>
#include <typeindex>

namespace VTX::App::Application::ECS
{
	using ComponentID = std::string;

	static const ComponentID INVALID_COMPONENT_ID = "";

	class ComponentInfo
	{
	  public:
		ComponentInfo() {}
		~ComponentInfo() {}

		template<Core::ECS::ECS_Component T>
		static void registerComponent( const ComponentID & p_id )
		{
			_mapTypeID[ typeid( T ).hash_code() ] = p_id;
		}

		template<Core::ECS::ECS_Component T>
		static const ComponentID & getComponentID()
		{
			const size_t hashcode = typeid( T ).hash_code();

			if ( !_mapTypeID.contains( hashcode ) )
				return INVALID_COMPONENT_ID;

			return _mapTypeID.at( hashcode );
		}

	  private:
		inline static std::map<size_t, ComponentID> _mapTypeID = std::map<size_t, ComponentID>();
	};

} // namespace VTX::App::Application::ECS
#endif
