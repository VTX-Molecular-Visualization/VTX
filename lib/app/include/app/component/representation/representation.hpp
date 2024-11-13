#ifndef __VTX_APP_COMPONENT_REPRESENTATION_REPRESENTATION__
#define __VTX_APP_COMPONENT_REPRESENTATION_REPRESENTATION__

#include "app/core/ecs/base_component.hpp"
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

		const VTX::Core::Struct::Representation & getRepresentation() const { return _representation; }

		template<Renderer::Proxy::E_REPRESENTATION_SETTINGS S, typename T>
		void set( const T p_value )
		{
			using namespace Renderer::Proxy;

			// Sphere.
			if constexpr ( S == E_REPRESENTATION_SETTINGS::HAS_SPHERE )
			{
				_representation.hasSphere = p_value;
			}
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_FIXED )
			{
				_representation.radiusSphereFixed = p_value;
			}
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_ADD )
			{
				_representation.radiusSphereAdd = p_value;
			}
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::IS_SPHERE_RADIUS_FIXED )
			{
				_representation.radiusFixed = p_value;
			}
			// Cylinder.
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::HAS_CYLINDER )
			{
				_representation.hasCylinder = p_value;
			}
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::RADIUS_CYLINDER )
			{
				_representation.radiusCylinder = p_value;
			}
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::CYLINDER_COLOR_BLENDING )
			{
				_representation.cylinderColorBlending = p_value;
			}
			// Ribbon.
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::HAS_RIBBON )
			{
				_representation.hasRibbon = p_value;
			}
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::RIBBON_COLOR_BLENDING )
			{
				_representation.ribbonColorBlending = p_value;
			}
			// SES.
			else if constexpr ( S == E_REPRESENTATION_SETTINGS::HAS_SES )
			{
				_representation.hasSes = p_value;
			}
			else
			{
				static_assert( true, "Unknown representation setting." );
			}

			// Trigger UI.
			callback<S, T>()( p_value );

			// Trigger renderer.
			_proxy->onChange<S, T>()( p_value );
		}

		template<Renderer::Proxy::E_REPRESENTATION_SETTINGS S, typename T>
		Util::Callback<const T> & callback()
		{
			static Util::Callback<const T> callback;
			return callback;
		}

	  private:
		VTX::Core::Struct::Representation _representation;
	};

} // namespace VTX::App::Component::Representation

#endif
