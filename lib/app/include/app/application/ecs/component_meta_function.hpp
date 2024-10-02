#ifndef __VTX_APP_APPLICATION_ECS_COMPONENT_META_FUNCTION__
#define __VTX_APP_APPLICATION_ECS_COMPONENT_META_FUNCTION__

#include "app/application/ecs/component_info.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/core/ecs/base_entity.hpp"
#include "app/core/ecs/concepts.hpp"
#include "app/core/serialization/serialization_system.hpp"
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
		ComponentMetaFunction() = default;
		~ComponentMetaFunction() {}

		template<Core::ECS::ECS_Component T>
		void registerComponent( const ComponentStaticID & p_id )
		{
			_mapSerializer[ p_id ]
				= []( const Core::ECS::BaseEntity & p_entity, const Application::ECS::RegistryManager & p_registry )
			{
				const T & component	 = p_registry.getComponent<T>( p_entity );
				auto &	  serializer = SERIALIZATION_SYSTEM();

				if ( serializer.canSerialize<T>() )
				{
					return serializer.serialize( component ).getObject();
				}
				else
				{
					return Util::JSon::Object();
				}
			};

			_mapDeserializer[ p_id ] = []( Application::ECS::RegistryManager & p_registry,
										   const Util::JSon::Object &		   p_json,
										   const Core::ECS::BaseEntity &	   p_target )
			{
				T &	   component  = p_registry.addComponent<T>( p_target );
				auto & serializer = SERIALIZATION_SYSTEM();

				if ( serializer.canSerialize<T>() )
				{
					serializer.deserialize( p_json, component );
				}
			};
		}

		const Util::JSon::Object serializeComponent(
			const Core::ECS::BaseEntity & p_target,
			const ComponentStaticID &	  p_id
		);
		void deserializeComponent(
			const ComponentStaticID &	  p_id,
			const Core::ECS::BaseEntity & p_target,
			const Util::JSon::Object &	  p_json
		);

	  private:
		using ComponentSerializerFunction = std::function<
			const Util::JSon::
				Object( const Core::ECS::BaseEntity &, const Application::ECS::RegistryManager &, const Core::Serialization::SerializationSystem & )>;

		using ComponentDeserializerFunction = std::function<
			void( Application::ECS::RegistryManager &, const Core::Serialization::SerializationSystem &, const Util::JSon::Object &, const Core::ECS::BaseEntity & )>;

		std::map<ComponentStaticID, ComponentSerializerFunction>   _mapSerializer;
		std::map<ComponentStaticID, ComponentDeserializerFunction> _mapDeserializer;
	};
} // namespace VTX::App::Application::ECS

#endif
