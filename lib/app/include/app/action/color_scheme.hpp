#ifndef __VTX_APP_ACTION_COLOR_SCHEME__
#define __VTX_APP_ACTION_COLOR_SCHEME__

#include "app/action/action_manager.hpp"
#include "app/ecs.hpp"
#include "app/services.hpp"
#include "app/system/color.hpp"
#include "app/system/color_scheme.hpp"
#include "app/system/uid.hpp"
#include <core/struct/system.hpp>
#include <renderer/color.hpp>
#include <renderer/renderer.hpp>
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
			using namespace Renderer::Color;

			auto &		 reg	= REG();
			const auto & system = reg.get<Core::Struct::System>( p_ent );
			const auto & uid	= REG().get<System::UID>( p_ent );
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
						color.atoms[ atom ] = getColorIndex( system.getAtomSymbol( atom ) );
					}
				}
			}
			else if constexpr ( S == System::E_COLOR_SCHEME::RESIDUE )
			{
				for ( Index residue : ranges )
				{
					const ColorIndex residueColorIndex = getColorIndex( system.getResidueSymbol( residue ) );
					const IndexRange range			   = system.getResidueAtomRange( residue );
					std::fill_n( color.atoms.begin() + range.getFirst(), range.getCount(), residueColorIndex );
				}
			}
			else if constexpr ( S == System::E_COLOR_SCHEME::CHAIN )
			{
				for ( Index residue : ranges )
				{
					const Index		 chainIndex		 = system.residueChainIndexes[ residue ];
					const ColorIndex chainColorIndex = getColorIndex( system.getChainName( chainIndex ) );
					const IndexRange range			 = system.getChainAtomRange( chainIndex );

					std::fill_n( color.atoms.begin() + range.getFirst(), range.getCount(), chainColorIndex );
				}

			} // TODO: other schemes.
			else
			{
				static_assert(
					always_false_v<S>, "Unsupported System::E_COLOR_SCHEME type in ColorScheme::Add action."
				);
			}

			// Push to renderer.
			RENDERER().setSystemColors( uid.system, color.atoms );
		}
	};

} // namespace VTX::App::Action::ColorScheme

#endif
