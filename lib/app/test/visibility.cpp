#include <app/action/visibility.hpp>
#include <app/fixture.hpp>
#include <app/helper/system.hpp>
#include <app/services.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/topology.hpp>
#include <optional>
#include <vector>

namespace
{
	using namespace VTX;
	using namespace Core::Struct;
	using namespace App;
	using namespace App::Action::Visibility;
	using namespace App::Helper::System;
	using namespace App::System;

	void _checkAtomRangeVisibility(
		const VTX::Util::Math::BitSet & p_visibility,
		const IndexRange &				p_range,
		const bool						p_visible
	)
	{
		for ( const VTX::Index atom : p_range )
		{
			CHECK( p_visibility.test( atom ) == p_visible );
		}
	}

	std::optional<VTX::Index> _findResidueWithMinimumAtomCount(
		const Topology & p_topology,
		const VTX::Index p_minCount
	)
	{
		for ( VTX::Index residue = 0; residue < p_topology.getResidueCount(); ++residue )
		{
			if ( p_topology.getResidueAtomCount( residue ) >= p_minCount )
			{
				return residue;
			}
		}

		return std::nullopt;
	}
} // namespace

TEST_CASE( "VTX_APP - Visibility - Loaded system starts visible", "[integration][visibility]" )
{
	using namespace VTX;

	Fixture app;
	app.loadSystem();
	const auto [ ent, topology, visibility ] = App::ECS::getFirstEntityWithComponents<Topology, System::Visibility>();

	REQUIRE( topology.getChainCount() >= 1 );
	REQUIRE( topology.getResidueCount() >= 1 );
	REQUIRE( topology.getAtomCount() >= 1 );

	CHECK( visibility.atoms.count() == topology.getAtomCount() );
	CHECK( getVisibleState( { ent, E_SYSTEM_ITEM::SYSTEM } ) == E_VISIBLE_STATE::VISIBLE );
	CHECK( getVisibleState( { ent, E_SYSTEM_ITEM::CHAIN, 0 } ) == E_VISIBLE_STATE::VISIBLE );
	CHECK( getVisibleState( { ent, E_SYSTEM_ITEM::RESIDUE, 0 } ) == E_VISIBLE_STATE::VISIBLE );
	CHECK( getVisibleState( { ent, E_SYSTEM_ITEM::ATOM, 0 } ) == E_VISIBLE_STATE::VISIBLE );
}

TEST_CASE( "VTX_APP - Visibility - Atom and residue actions target the correct atoms", "[integration][visibility]" )
{
	using namespace VTX;

	Fixture app;
	app.loadSystem();
	const auto [ system, topology, visibility ] = App::ECS::getFirstEntityWithComponents<Topology, System::Visibility>();
	const auto residueIndexOpt = _findResidueWithMinimumAtomCount( topology, 2 );

	REQUIRE( residueIndexOpt.has_value() );

	const Index		 residueIndex = *residueIndexOpt;
	const IndexRange residueAtoms = topology.getResidueAtomRange( residueIndex );
	const Index		 hiddenAtom	  = residueAtoms.getFirst();
	const Index		 otherAtom	  = hiddenAtom + 1;

	SECTION( "single atom overload only toggles the requested atom" )
	{
		ACTION().execute<SetVisible<E_SYSTEM_ITEM::ATOM>>( system, hiddenAtom, false );

		const auto & updatedVisibility = REG().get<System::Visibility>( system );
		CHECK_FALSE( updatedVisibility.atoms.test( hiddenAtom ) );
		CHECK( updatedVisibility.atoms.test( otherAtom ) );
		CHECK( updatedVisibility.atoms.count() == topology.getAtomCount() - 1 );

		CHECK( getVisibleState( { system, E_SYSTEM_ITEM::ATOM, hiddenAtom } ) == E_VISIBLE_STATE::HIDDEN );
		CHECK( getVisibleState( { system, E_SYSTEM_ITEM::RESIDUE, residueIndex } ) == E_VISIBLE_STATE::PARTIAL );
		CHECK( getVisibleState( { system, E_SYSTEM_ITEM::SYSTEM } ) == E_VISIBLE_STATE::PARTIAL );

		ACTION().execute<SetVisible<E_SYSTEM_ITEM::ATOM>>(
			system, std::vector<Index> { hiddenAtom }, true
		);

		CHECK( REG().get<System::Visibility>( system ).atoms.count() == topology.getAtomCount() );
		CHECK( getVisibleState( { system, E_SYSTEM_ITEM::ATOM, hiddenAtom } ) == E_VISIBLE_STATE::VISIBLE );
	}

	SECTION( "residue range overload expands residue indexes into atom ranges" )
	{
		REQUIRE( topology.getResidueCount() >= 3 );

		const IndexRange hiddenResidues( 0, 2 );
		const IndexRange firstResidueAtoms	= topology.getResidueAtomRange( 0 );
		const IndexRange secondResidueAtoms = topology.getResidueAtomRange( 1 );
		const IndexRange thirdResidueAtoms	= topology.getResidueAtomRange( 2 );

		ACTION().execute<SetVisible<E_SYSTEM_ITEM::RESIDUE>>(
			system, hiddenResidues, false
		);

		const auto & updatedVisibility = REG().get<System::Visibility>( system );
		_checkAtomRangeVisibility( updatedVisibility.atoms, firstResidueAtoms, false );
		_checkAtomRangeVisibility( updatedVisibility.atoms, secondResidueAtoms, false );
		_checkAtomRangeVisibility( updatedVisibility.atoms, thirdResidueAtoms, true );

		CHECK( getVisibleState( { system, E_SYSTEM_ITEM::RESIDUE, 0 } ) == E_VISIBLE_STATE::HIDDEN );
		CHECK( getVisibleState( { system, E_SYSTEM_ITEM::RESIDUE, 1 } ) == E_VISIBLE_STATE::HIDDEN );
		CHECK( getVisibleState( { system, E_SYSTEM_ITEM::SYSTEM } ) == E_VISIBLE_STATE::PARTIAL );

		ACTION().execute<SetVisible<E_SYSTEM_ITEM::RESIDUE>>(
			system, hiddenResidues, true
		);

		CHECK( REG().get<System::Visibility>( system ).atoms.count() == topology.getAtomCount() );
		CHECK( getVisibleState( { system, E_SYSTEM_ITEM::RESIDUE, 0 } ) == E_VISIBLE_STATE::VISIBLE );
		CHECK( getVisibleState( { system, E_SYSTEM_ITEM::RESIDUE, 1 } ) == E_VISIBLE_STATE::VISIBLE );
	}
}

TEST_CASE( "VTX_APP - Visibility - SetVisibleItem dispatches chain and system updates", "[integration][visibility]" )
{
	using namespace VTX;

	Fixture app;
	app.loadSystem();
	const auto [ system, topology, visibility ] = App::ECS::getFirstEntityWithComponents<Topology, System::Visibility>();

	REQUIRE( topology.getChainCount() >= 1 );

	const Index		   chainIndex = 0;
	const IndexRange   chainAtoms = topology.getChainAtomRange( chainIndex );
	const E_VISIBLE_STATE systemAfterChainHide
		= chainAtoms.getCount() == topology.getAtomCount() ? E_VISIBLE_STATE::HIDDEN : E_VISIBLE_STATE::PARTIAL;

	ACTION().execute<SetVisibleItem>( system, E_SYSTEM_ITEM::CHAIN, chainIndex, false );

	_checkAtomRangeVisibility( REG().get<System::Visibility>( system ).atoms, chainAtoms, false );
	CHECK( getVisibleState( { system, E_SYSTEM_ITEM::CHAIN, chainIndex } ) == E_VISIBLE_STATE::HIDDEN );
	CHECK( getVisibleState( { system, E_SYSTEM_ITEM::SYSTEM } ) == systemAfterChainHide );

	ACTION().execute<SetVisibleItem>( system, E_SYSTEM_ITEM::CHAIN, chainIndex, true );

	CHECK( getVisibleState( { system, E_SYSTEM_ITEM::CHAIN, chainIndex } ) == E_VISIBLE_STATE::VISIBLE );
	CHECK( getVisibleState( { system, E_SYSTEM_ITEM::SYSTEM } ) == E_VISIBLE_STATE::VISIBLE );

	const IndexRangeList noItemRanges;
	ACTION().execute<SetVisibleItem>( system, E_SYSTEM_ITEM::SYSTEM, noItemRanges, false );

	CHECK( REG().get<System::Visibility>( system ).atoms.none() );
	CHECK( getVisibleState( { system, E_SYSTEM_ITEM::SYSTEM } ) == E_VISIBLE_STATE::HIDDEN );

	ACTION().execute<SetVisibleItem>( system, E_SYSTEM_ITEM::SYSTEM, noItemRanges, true );

	CHECK( REG().get<System::Visibility>( system ).atoms.count() == topology.getAtomCount() );
	CHECK( getVisibleState( { system, E_SYSTEM_ITEM::SYSTEM } ) == E_VISIBLE_STATE::VISIBLE );
}
