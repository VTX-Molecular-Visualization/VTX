#ifndef __VTX_APP_ACTION_COLOR_SCHEME__
#define __VTX_APP_ACTION_COLOR_SCHEME__

#include "app/ecs.hpp"
#include "app/system/color.hpp"
#include "app/system/uid.hpp"
#include <core/struct/system.hpp>
#include <renderer/color.hpp>
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
				= p_ranges.isEmpty() ? Core::Struct::IndexRangeList( { { 0, system.getAtomCount() } } ) : p_ranges;

			reg.patch<System::Color>(
				p_ent,
				[ & ]( System::Color & p_color )
				{
					// Merge ranges.
					if ( not color.colorSchemeAtoms.contains( S ) )
					{
						p_color.colorSchemeAtoms.emplace( S, Core::Struct::IndexRangeList() );
					}

					for ( auto & [ scheme, rangeList ] : p_color.colorSchemeAtoms )
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
							p_color.atoms[ atom ] = getColorIndex( system.getAtomSymbol( atom ) );
						}
					}
					else if constexpr ( S == System::E_COLOR_SCHEME::RESIDUE )
					{
						for ( Index atom : ranges )
						{
							const Index residue	  = system.atomResidueIndexes[ atom ];
							p_color.atoms[ atom ] = getColorIndex( system.getAtomSymbol( residue ) );
						}
					}
					else if constexpr ( S == System::E_COLOR_SCHEME::CHAIN )
					{
						for ( Index atom : ranges )
						{
							const Index residue	  = system.atomResidueIndexes[ atom ];
							const Index chain	  = system.residueChainIndexes[ residue ];
							p_color.atoms[ atom ] = getColorIndex( system.getChainName( chain ) );
						}
					}
					// TODO: other schemes.
					else
					{
						static_assert(
							always_false_v<S>, "Unsupported System::E_COLOR_SCHEME type in ColorScheme::Add action."
						);
					}
				}
			);
		}
	};

} // namespace VTX::App::Action::Color

#endif
