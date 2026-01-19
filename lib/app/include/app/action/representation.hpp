#ifndef __VTX_APP_ACTION_REPRESENTATION__
#define __VTX_APP_ACTION_REPRESENTATION__

#include "app/ecs.hpp"
#include <core/struct/system.hpp>
#include <renderer/representation.hpp>
#include <util/type_traits.hpp>

namespace VTX::App::Action::Representation
{
	/**
	 * @brief Change a representation value.
	 */
	template<VTX::Renderer::E_REPRESENTATION_VALUES S, typename T>
	struct Change
	{
		void execute( const ECS::Entity p_e, const T & p_value )
		{
			REG().patch<Renderer::Representation>(
				p_e,
				[ p_value ]( Renderer::Representation & p_representation )
				{
					if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::HAS_SPHERE )
					{
						p_representation.hasSphere = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::RADIUS_SPHERE_FIXED )
					{
						p_representation.radiusSphereFixed = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::RADIUS_SPHERE_ADD )
					{
						p_representation.radiusSphereAdd = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::IS_SPHERE_RADIUS_FIXED )
					{
						p_representation.isRadiusSphereFixed = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::HAS_CYLINDER )
					{
						p_representation.hasCylinder = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::RADIUS_CYLINDER )
					{
						p_representation.radiusCylinder = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::CYLINDER_COLOR_BLENDING )
					{
						p_representation.cylinderColorBlending = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::HAS_RIBBON )
					{
						p_representation.hasRibbon = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::RIBBON_COLOR_BLENDING )
					{
						p_representation.ribbonColorBlending = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::HAS_SES )
					{
						p_representation.hasSes = p_value;
					}
					else if constexpr ( S == Renderer::E_REPRESENTATION_VALUES::SES_PROBE_RADIUS )
					{
						p_representation.sesProbeRadius = p_value;
					}
					else
					{
						static_assert( always_false_v<S>, "Representation::Change: invalid representation value." );
					}
				}
			);
		}
	};

	/**
	 * @brief Add a representation to a system.
	 */
	struct AddToSystem
	{
		void execute(
			const ECS::Entity p_system,
			const ECS::Entity p_preset,
			const Core::Struct::IndexRangeList & = {}
		);
	};
} // namespace VTX::App::Action::Representation
#endif
