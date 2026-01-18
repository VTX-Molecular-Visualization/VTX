#ifndef __VTX_APP_ACTION_COLOR_SCHEME__
#define __VTX_APP_ACTION_COLOR_SCHEME__

#include "app/action/action_manager.hpp"
#include "app/ecs.hpp"
#include "app/services.hpp"
#include "app/system/color.hpp"
#include "app/system/uid.hpp"
#include <core/struct/system.hpp>
#include <renderer/color.hpp>
#include <renderer/renderer.hpp>
#include <util/type_traits.hpp>

namespace VTX::App::Action::Color
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

			IndexRangeList ranges
				= p_ranges.isEmpty() ? Core::Struct::IndexRangeList( { { 0, system.getResidueCount() } } ) : p_ranges;

			// Merge ranges.
			if ( not color.colorSchemeAtoms.contains( S ) )
			{
				color.colorSchemeAtoms.emplace( S, Core::Struct::IndexRangeList() );
			}

			for ( auto & [ scheme, rangeList ] : color.colorSchemeAtoms )
			{
				if ( scheme == S )
				{
					rangeList.mergeInPlace( ranges );
				}
				else
				{
					rangeList.substractInPlace( ranges );
				}
			}

			// Apply color scheme.
			if constexpr ( S == System::E_COLOR_SCHEME::ATOM )
			{
				for ( Index atom : ranges )
				{
					color.atoms[ atom ] = getColorIndex( system.getAtomSymbol( atom ) );
				}
			}
			else if constexpr ( S == System::E_COLOR_SCHEME::RESIDUE )
			{
				for ( Index atom : ranges )
				{
					const Index		 residue		   = system.atomResidueIndexes[ atom ];
					const ColorIndex residueColorIndex = getColorIndex( system.getResidueSymbol( residue ) );
					const IndexRange range			   = system.getResidueAtomRange( residue );
					std::fill_n( color.atoms.begin() + range.getFirst(), range.getCount(), residueColorIndex );
				}
			}
			else if constexpr ( S == System::E_COLOR_SCHEME::CHAIN )
			{
				for ( Index atom : ranges )
				{
					const Index		 residue		 = system.atomResidueIndexes[ atom ];
					const Index		 chain			 = system.residueChainIndexes[ residue ];
					const ColorIndex chainColorIndex = getColorIndex( system.getChainName( chain ) );
					const IndexRange range			 = system.getChainAtomRange( chain );
					std::fill_n( color.atoms.begin() + range.getFirst(), range.getCount(), chainColorIndex );
				}
			}
			// TODO: other schemes.
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

} // namespace VTX::App::Action::Color

#endif
