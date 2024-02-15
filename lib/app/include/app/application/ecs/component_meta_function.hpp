#ifndef __VTX_APP_APPLICATION_ECS_COMPONENT_META_FUNCTION__
#define __VTX_APP_APPLICATION_ECS_COMPONENT_META_FUNCTION__

#include "app/application/ecs/component_info.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/application/system/system_registration.hpp"
#include "app/core/ecs/base_entity.hpp"
#include "app/core/ecs/concepts.hpp"
#include "app/core/serialization/serialization.hpp"
#include "app/core/system/base_system.hpp"
#include <functional>
#include <map>
#include <string>
#include <typeindex>
#include <util/json/json.hpp>

namespace VTX::App::Application::ECS
{
	class ComponentMetaFunction : public Core::System::BaseSystem
	{
	  public:
		inline static const System::SystemRegistration<ComponentMetaFunction> SYSTEM_REG
			= System::SystemRegistration<ComponentMetaFunction>();

	  public:
		ComponentMetaFunction() = default;
		~ComponentMetaFunction() {}

		template<Core::ECS::ECS_Component T>
		void registerComponent( const ComponentID & p_id )
		{
			_mapSerializer[ p_id ] = [ p_id ](
										 const Core::ECS::BaseEntity &				p_entity,
										 const Application::ECS::RegistryManager &	p_registry,
										 const Core::Serialization::Serialization & p_serializer
									 )
			{
				const T & component = p_registry.getComponent<T>( p_entity );

				if ( p_serializer.canSerialize<T>() )
				{
					return p_serializer.serialize( component ).getObject();
				}
				else
				{
					return Util::JSon::Object();
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

		const Util::JSon::Object serializeComponent( const Core::ECS::BaseEntity & p_target, const ComponentID & p_id );
		void					 deserializeComponent(
								const ComponentID &			  p_id,
								const Core::ECS::BaseEntity & p_target,
								const Util::JSon::Object &	  p_json
							);

	  private:
		using ComponentSerializerFunction = std::function<
			const Util::JSon::
				Object( const Core::ECS::BaseEntity &, const Application::ECS::RegistryManager &, const Core::Serialization::Serialization & )>;

		using ComponentDeserializerFunction = std::function<
			void( Application::ECS::RegistryManager &, const Core::Serialization::Serialization &, const Util::JSon::Object &, const Core::ECS::BaseEntity & )>;

		std::map<ComponentID, ComponentSerializerFunction> _mapSerializer
			= std::map<ComponentID, ComponentSerializerFunction>();
		std::map<ComponentID, ComponentDeserializerFunction> _mapDeserializer
			= std::map<ComponentID, ComponentDeserializerFunction>();
	};
} // namespace VTX::App::Application::ECS

namespace VTX::App
{
	Application::ECS::ComponentMetaFunction & COMPONENT_META_FUNCTION();
}
#endif
