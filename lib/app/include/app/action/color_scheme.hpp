#ifndef __VTX_APP_ACTION_COLOR_SCHEME__
#define __VTX_APP_ACTION_COLOR_SCHEME__

#include "app/action/action_manager.hpp"
#include "app/ecs.hpp"
#include "app/services.hpp"
#include "app/system/color.hpp"
#include "app/system/color_scheme.hpp"
#include <core/struct/system.hpp>
#include <renderer/color.hpp>
#include <util/type_traits.hpp>

namespace VTX::App::Action::ColorScheme
{

	/**
	 * @brief Set item selection.
	 */
	template<System::E_COLOR_SCHEME S>
	struct Add
	{
		void execute( const ECS::Entity p_ent, const Core::Struct::IndexRangeList & p_ranges = {} )
		{
			using namespace Core::Struct;
			using namespace Renderer;

			auto &		 reg	= REG();
			const auto & system = reg.get<Core::Struct::System>( p_ent );
			auto &		 color	= reg.get<System::Color>( p_ent );

			auto & scheme = reg.emplace<System::ColorScheme>( p_ent );

			IndexRangeList ranges
				= p_ranges.isEmpty() ? Core::Struct::IndexRangeList( { { 0, system.getResidueCount() } } ) : p_ranges;

			scheme.scheme	= S;
			scheme.residues = ranges;

			// TODO: merge with other ColorSchemes.

			if constexpr ( S == System::E_COLOR_SCHEME::ATOM )
			{
				for ( Index residue : ranges )
				{
					for ( Index atom : system.getResidueAtomRange( residue ) )
					{
						color.atoms[ atom ] = Color::getColorIndex( system.getAtomSymbol( atom ) );
					}
				}
			}
			// TODO: other schemes.
			else
			{
				static_assert(
					always_false_v<S>, "Unsupported System::E_COLOR_SCHEME type in ColorScheme::Add action."
				);
			}

			/*
			reg.patch<System::Selection>(
				p_ent, [ selectionAtoms ]( System::Selection & p_selection ) { p_selection.atoms = selectionAtoms; }
			);
			*/
		}
	};

} // namespace VTX::App::Action::ColorScheme

#endif
