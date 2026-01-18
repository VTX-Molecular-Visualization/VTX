#include "app/action/representation.hpp"
#include "app/services.hpp"
#include "app/system/representation.hpp"
#include "app/system/uid.hpp"
#include <core/struct/system.hpp>
#include <renderer/renderer.hpp>
#include <renderer/representation.hpp>
#include <util/exceptions.hpp>

namespace VTX::App::Action::Representation
{

	void AddToSystem::execute(
		const ECS::Entity					 p_system,
		const ECS::Entity					 p_preset,
		const Core::Struct::IndexRangeList & p_ranges
	)
	{
		using namespace Core::Struct;

		auto &		 reg			= REG();
		const auto & system			= reg.get<Core::Struct::System>( p_system );
		const auto & uid			= REG().get<System::UID>( p_system );
		auto &		 representation = reg.get<System::Representation>( p_system );

		IndexRangeList ranges
			= p_ranges.isEmpty() ? Core::Struct::IndexRangeList( { { 0, system.getResidueCount() } } ) : p_ranges;

		// Merges ranges.
		if ( not representation.presetAtoms.contains( p_preset ) )
		{
			representation.presetAtoms.emplace( p_preset, Core::Struct::IndexRangeList() );
		}

		for ( auto it = representation.presetAtoms.begin(); it != representation.presetAtoms.end(); )
		{
			auto & [ entity, rangeList ] = *it;

			if ( entity == p_preset )
			{
				rangeList.mergeInPlace( ranges );
				++it;
			}
			else
			{
				rangeList.substractInPlace( ranges );
				if ( rangeList.isEmpty() )
				{
					it = representation.presetAtoms.erase( it );
				}
				else
				{
					++it;
				}
			}
		}

		// Set index.
		for ( auto it = ranges.rangeBegin(); it != ranges.rangeEnd(); ++it )
		{
			// TODO: use handle for representation presets.
			std::fill_n( representation.atoms.begin() + it->getFirst(), it->getCount(), 0 );
		}

		// Push to renderer.
		RENDERER().setSystemRepresentation( uid.system, representation.atoms );
	}

} // namespace VTX::App::Action::Representation
