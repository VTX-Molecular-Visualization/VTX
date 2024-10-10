#ifndef __VTX_APP_APPLICATION_ECS_COMPONENT_STATIC_ID_MAP__
#define __VTX_APP_APPLICATION_ECS_COMPONENT_STATIC_ID_MAP__

#include "app/core/ecs/concepts.hpp"
#include <map>
#include <string>
#include <typeinfo>

namespace VTX::App::Application::ECS
{
	using ComponentStaticID = std::string;

	const ComponentStaticID INVALID_COMPONENT_STATIC_ID = "";

	class ComponentStaticIDMap
	{
	  public:
		ComponentStaticIDMap() = default;
		~ComponentStaticIDMap() {}

		template<Core::ECS::ConceptComponent T>
		void registerComponent( const ComponentStaticID & p_id )
		{
			_mapTypeStaticID[ typeid( T ).hash_code() ] = p_id;
		}

		template<Core::ECS::ConceptComponent T>
		const ComponentStaticID & getComponentID()
		{
			const size_t hashcode = typeid( T ).hash_code();

			if ( !_mapTypeStaticID.contains( hashcode ) )
				return INVALID_COMPONENT_STATIC_ID;

			return _mapTypeStaticID.at( hashcode );
		}

	  private:
		std::map<size_t, ComponentStaticID> _mapTypeStaticID = std::map<size_t, ComponentStaticID>();
	};

} // namespace VTX::App::Application::ECS

#endif
