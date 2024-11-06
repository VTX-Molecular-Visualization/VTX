#ifndef __VTX_APP_COMPONENT_REPRESENTATION_REPRESENTATION__
#define __VTX_APP_COMPONENT_REPRESENTATION_REPRESENTATION__

#include "app/core/ecs/base_component.hpp"
#include <core/chemdb/representation.hpp>
#include <core/struct/representation.hpp>
#include <renderer/proxy/representation.hpp>
#include <util/callback.hpp>

namespace VTX::App::Component::Representation
{

	class Representation : public Core::ECS::BaseComponentProxy<Renderer::Proxy::Representation>
	{
	  public:
		Representation();

		void setupProxy() override;

		const VTX::Core::Struct::Representation & getRepresentation() const { return _settings; }

		template<VTX::Core::ChemDB::Representation::E_REPRESENTATION_SETTINGS S, typename T>
		void set( const T p_value )
		{
			// Sphere.
			if constexpr ( S == VTX::Core::ChemDB::Representation::E_REPRESENTATION_SETTINGS::ACTIVE_SSAO )
			{
				//_settings.activeSSAO = p_value;
			}

			// Trigger UI.
			callback<S, T>()( p_value );

			// Trigger renderer.
			_proxy->onChange<S, T>()( p_value );
		}

		template<VTX::Core::ChemDB::Representation::E_REPRESENTATION_SETTINGS S, typename T>
		Util::Callback<const T> & callback()
		{
			static Util::Callback<const T> callback;
			return callback;
		}

	  private:
		VTX::Core::Struct::Representation _settings;
	};

} // namespace VTX::App::Component::Representation

#endif
