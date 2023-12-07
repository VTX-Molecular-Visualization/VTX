#ifndef __VTX_APP_APPLICATION_ECS_COMPONENT_META_FUNCTION__
#define __VTX_APP_APPLICATION_ECS_COMPONENT_META_FUNCTION__

#include "app/application/ecs/component_info.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/core/ecs/base_entity.hpp"
#include "app/core/ecs/concepts.hpp"
#include "app/core/serialization/serialization.hpp"
#include <functional>
#include <map>
#include <string>
#include <typeindex>
#include <util/json/json.hpp>

namespace VTX::App::Application::ECS
{
	class ComponentMetaFunction
	{
	  public:
		ComponentMetaFunction() {}
		~ComponentMetaFunction() {}

		template<Core::ECS::ECS_Component T>
		static void registerComponent( const ComponentID & p_id )
		{
			_mapSerializer[ p_id ] = [ p_id ](
										 const Core::ECS::BaseEntity &				p_entity,
										 Util::JSon::Object &						p_jsonEntity,
										 const Application::ECS::RegistryManager &	p_registry,
										 const Core::Serialization::Serialization & p_serializer
									 )
			{
				const T & component = p_registry.getComponent<T>( p_entity );

				if ( p_serializer.canSerialize<T>() )
				{
					p_jsonEntity.appendField( p_id, p_serializer.serialize( component ) );
				}
				else
				{
					p_jsonEntity.appendField( p_id, Util::JSon::Object() );
				}
			};

			_mapDeserializer[ p_id ] = []( Application::ECS::RegistryManager &		  p_registry,
										   const Core::Serialization::Serialization & p_serializer,
										   const Util::JSon::Object &				  p_json,
										   const Core::ECS::BaseEntity &			  p_target )
			{
				T & component = p_registry.addComponent<T>( p_target );

				if ( p_serializer.canSerialize<T>() )
				{
					p_serializer.deserialize( p_json, component );
				}
			};
		}

		static void serializeComponent(
			const Core::ECS::BaseEntity & p_target,
			const ComponentID &			  p_id,
			Util::JSon::Object &		  p_json
		);
		static void deserializeComponent(
			const ComponentID &			  p_id,
			const Core::ECS::BaseEntity & p_target,
			const Util::JSon::Object &	  p_json
		);

	  private:
		using ComponentSerializerFunction = std::function<
			void( const Core::ECS::BaseEntity &, Util::JSon::Object &, const Application::ECS::RegistryManager &, const Core::Serialization::Serialization & )>;

		using ComponentDeserializerFunction = std::function<
			void( Application::ECS::RegistryManager &, const Core::Serialization::Serialization &, const Util::JSon::Object &, const Core::ECS::BaseEntity & )>;

		inline static std::map<ComponentID, ComponentSerializerFunction> _mapSerializer
			= std::map<ComponentID, ComponentSerializerFunction>();
		inline static std::map<ComponentID, ComponentDeserializerFunction> _mapDeserializer
			= std::map<ComponentID, ComponentDeserializerFunction>();
	};
} // namespace VTX::App::Application::ECS
#endif
