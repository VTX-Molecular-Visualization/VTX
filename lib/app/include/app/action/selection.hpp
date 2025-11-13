#ifndef __VTX_APP_ACTION_SELECTION__
#define __VTX_APP_ACTION_SELECTION__

#include "app/ecs.hpp"
#include <util/math/range_list.hpp>
#include <util/types.hpp>

namespace VTX::App::Action::Selection
{
	struct SelectSystem
	{
		void execute( const ECS::Entity, const bool = true );
	};

	struct SelectChain
	{
		void execute( const ECS::Entity, const Index, const bool = true );
	};

	struct SelectChains
	{
		void execute( const ECS::Entity, const Util::Math::RangeList<Index> &, const bool = true );
	};

	struct SelectResidue
	{
		void execute( const ECS::Entity, const Index, const bool = true );
	};

	struct SelectResidues
	{
		void execute( const ECS::Entity, const Util::Math::RangeList<Index> &, const bool = true );
	};

	struct SelectAtom
	{
		void execute( const ECS::Entity, const Index, const bool = true );
	};

	struct SelectAtoms
	{
		void execute( const ECS::Entity, const Util::Math::RangeList<Index> &, const bool = true );
	};

	struct Select
	{
		struct SelectionData
		{
			Util::Math::RangeList<Index> chains;
			Util::Math::RangeList<Index> residues;
			Util::Math::RangeList<Index> atoms;
		};

		void execute(
			const ECS::Entity,
			const SelectionData &				 p_add,
			const std::optional<SelectionData> & p_remove = std::nullopt
		);
	};
} // namespace VTX::App::Action::Selection

#endif
