#include "app/action/action_manager.hpp"
#include "app/services.hpp"
#include <app/action/selection.hpp>
#include <app/fixture.hpp>
#include <app/helper/system.hpp>
#include <app/services.hpp>
#include <app/system/trajectory.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/topology.hpp>
#include <optional>
#include <vector>

namespace
{
	using namespace VTX;
	using namespace Core::Struct;
	using namespace App;
	using namespace App::Action::Selection;
	using namespace App::Helper::System;
	using namespace App::System;

	void _checkAtomRangeSelection(
		const VTX::Util::Math::BitSet & p_selection,
		const IndexRange &				p_range,
		const bool						p_selected
	)
	{
		for ( const VTX::Index atom : p_range )
		{
			CHECK( p_selection.test( atom ) == p_selected );
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

TEST_CASE( "VTX_APP - Selection - Extend selection to 4 angstrom", "[extendSelection]" )
{
	using namespace VTX;
	App::Fixture app;
	app.loadSystem( VTX::FilePath( Util::Filesystem::getExecutableDir() / "data" / "6fxo.bcif.gz" ) );
	for ( auto system : App::REG().view<Core::Struct::Topology>() )
	{
		const Index toSelect { 1667 };
		App::ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( system, toSelect );
	}
	app.loadSystem( VTX::FilePath( Util::Filesystem::getExecutableDir() / "data" / "4hhb.bcif.gz" ) );

	const std::vector<int> refValue { 7, 28, 83 };

	for ( int i = 0; i < 3; i++ )
	{
		App::ACTION().execute<App::Action::Selection::ExtendSelectionSelecFirst>( 4.0 );
		int count { 0 };
		int size { 0 };
		for ( auto system : App::REG().view<Core::Struct::Topology>() )
		{
			const auto & selection = App::REG().get<App::System::Selection>( system );
			size += selection.atoms.size();

			for ( size_t j : selection.atoms )
				count++;
		}
		CHECK( count == refValue[ i ] );
	}
}

TEST_CASE( "VTX_APP - Selection - Extend selection to residues", "[extendSelection]" )
{
	using namespace VTX;
	App::Fixture app;
	app.loadSystem( VTX::FilePath( Util::Filesystem::getExecutableDir() / "data" / "6fxo.bcif.gz" ) );
	app.loadSystem( VTX::FilePath( Util::Filesystem::getExecutableDir() / "data" / "4hhb.bcif.gz" ) );
	for ( auto system : App::REG().view<Core::Struct::Topology>() )
	{
		Core::Struct::IndexRangeList toSelect;
		toSelect.addRange( 0 );
		toSelect.addRange( 1758 );
		App::ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( system, toSelect );
	}

	App::ACTION().execute<App::Action::Selection::ExtendSelectionRes>();

	int count { 0 };
	for ( auto system : App::REG().view<Core::Struct::Topology>() )
	{
		const auto & selection = App::REG().get<App::System::Selection>( system );
		for ( size_t j : selection.atoms )
			count++;
	}
	CHECK( count == 35 );
}

TEST_CASE( "VTX_APP - Selection - RevertSelection", "[extendSelection]" )
{
	using namespace VTX;
	App::Fixture app;
	app.loadSystem( VTX::FilePath( Util::Filesystem::getExecutableDir() / "data" / "6fxo.bcif.gz" ) );
	for ( auto system : App::REG().view<Core::Struct::Topology>() )
	{
		Index toSelect { 0 };
		App::ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( system, toSelect );
	}
	app.loadSystem( VTX::FilePath( Util::Filesystem::getExecutableDir() / "data" / "4hhb.bcif.gz" ) );

	App::ACTION().execute<App::Action::Selection::RevertSelection>();

	int count { 0 };
	int L { 0 };
	for ( auto system : App::REG().view<Core::Struct::Topology>() )
	{
		auto & selection = App::REG().get<App::System::Selection>( system );
		L += selection.atoms.size();
		for ( size_t j : selection.atoms )
			count++;
	}
	CHECK( count == L - 1 );

	App::ACTION().execute<Clear>();
	for ( auto system : App::REG().view<Core::Struct::Topology>() )
	{
		Core::Struct::IndexRange toSelect { 0, 1200 };
		App::ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( system, toSelect );
	}

	App::ACTION().execute<App::Action::Selection::RevertSelection>();

	count = 0;
	for ( auto system : App::REG().view<Core::Struct::Topology>() )
	{
		auto & selection = App::REG().get<App::System::Selection>( system );
		for ( size_t j : selection.atoms )
			count++;
	}
	CHECK( count == L - 2400 );
}

TEST_CASE( "VTX_APP - Selection - Loaded system starts empty", "[integration][selection]" )
{
	using namespace VTX;

	Fixture app;
	app.loadSystem();
	const auto [ ent, topology, selection ] = App::ECS::getFirstEntityWithComponents<Topology, System::Selection>();

	REQUIRE( topology.getChainCount() >= 1 );
	REQUIRE( topology.getResidueCount() >= 1 );
	REQUIRE( topology.getAtomCount() >= 1 );

	CHECK( selection.atoms.none() );
	CHECK( selection.atoms.count() == 0 );
	CHECK( getSelectionState( { ent, E_SYSTEM_ITEM::SYSTEM } ) == E_SELECTION_STATE::NONE );
	CHECK( getSelectionState( { ent, E_SYSTEM_ITEM::CHAIN, 0 } ) == E_SELECTION_STATE::NONE );
	CHECK( getSelectionState( { ent, E_SYSTEM_ITEM::RESIDUE, 0 } ) == E_SELECTION_STATE::NONE );
	CHECK( getSelectionState( { ent, E_SYSTEM_ITEM::ATOM, 0 } ) == E_SELECTION_STATE::NONE );
}

TEST_CASE( "VTX_APP - Selection - SetSelected maps items to atom selections", "[integration][selection]" )
{
	using namespace VTX;

	Fixture app;
	app.loadSystem();
	const auto [ system, topology, selection ] = App::ECS::getFirstEntityWithComponents<Topology, System::Selection>();

	SECTION( "atom overloads support replace, append and subtract" )
	{
		const auto residueIndexOpt = _findResidueWithMinimumAtomCount( topology, 2 );
		REQUIRE( residueIndexOpt.has_value() );

		const Index		 residueIndex = *residueIndexOpt;
		const IndexRange residueAtoms = topology.getResidueAtomRange( residueIndex );
		const Index		 firstAtom	  = residueAtoms.getFirst();
		const Index		 secondAtom	  = firstAtom + 1;

		ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( system, firstAtom );

		CHECK( REG().get<System::Selection>( system ).atoms.count() == 1 );
		CHECK( getSelectionState( { system, E_SYSTEM_ITEM::ATOM, firstAtom } ) == E_SELECTION_STATE::FULL );
		CHECK( getSelectionState( { system, E_SYSTEM_ITEM::RESIDUE, residueIndex } ) == E_SELECTION_STATE::PARTIAL );
		CHECK( getSelectionState( { system, E_SYSTEM_ITEM::SYSTEM } ) == E_SELECTION_STATE::PARTIAL );

		ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( system, std::vector<Index> { secondAtom }, true, true );

		const auto & updatedSelection = REG().get<System::Selection>( system );
		CHECK( updatedSelection.atoms.test( firstAtom ) );
		CHECK( updatedSelection.atoms.test( secondAtom ) );
		CHECK( updatedSelection.atoms.count() == 2 );

		ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( system, firstAtom, false, true );

		CHECK_FALSE( REG().get<System::Selection>( system ).atoms.test( firstAtom ) );
		CHECK( REG().get<System::Selection>( system ).atoms.test( secondAtom ) );
		CHECK( REG().get<System::Selection>( system ).atoms.count() == 1 );
	}

	SECTION( "residue range overload expands residue indexes into atom ranges" )
	{
		REQUIRE( topology.getResidueCount() >= 3 );

		const IndexRange selectedResidues( 0, 2 );
		const IndexRange firstResidueAtoms	= topology.getResidueAtomRange( 0 );
		const IndexRange secondResidueAtoms = topology.getResidueAtomRange( 1 );
		const IndexRange thirdResidueAtoms	= topology.getResidueAtomRange( 2 );

		ACTION().execute<SetSelected<E_SYSTEM_ITEM::RESIDUE>>( system, selectedResidues );

		const auto & updatedSelection = REG().get<System::Selection>( system );
		_checkAtomRangeSelection( updatedSelection.atoms, firstResidueAtoms, true );
		_checkAtomRangeSelection( updatedSelection.atoms, secondResidueAtoms, true );
		_checkAtomRangeSelection( updatedSelection.atoms, thirdResidueAtoms, false );

		CHECK( getSelectionState( { system, E_SYSTEM_ITEM::RESIDUE, 0 } ) == E_SELECTION_STATE::FULL );
		CHECK( getSelectionState( { system, E_SYSTEM_ITEM::RESIDUE, 1 } ) == E_SELECTION_STATE::FULL );
		CHECK( getSelectionState( { system, E_SYSTEM_ITEM::RESIDUE, 2 } ) == E_SELECTION_STATE::NONE );
		CHECK( getSelectionState( { system, E_SYSTEM_ITEM::SYSTEM } ) == E_SELECTION_STATE::PARTIAL );

		ACTION().execute<SetSelected<E_SYSTEM_ITEM::RESIDUE>>( system, Index( 0 ), false, true );

		_checkAtomRangeSelection( REG().get<System::Selection>( system ).atoms, firstResidueAtoms, false );
		_checkAtomRangeSelection( REG().get<System::Selection>( system ).atoms, secondResidueAtoms, true );
	}

	SECTION( "system selection uses the default empty range list as full-system selection" )
	{
		const IndexRangeList noRanges;
		ACTION().execute<SetSelected<E_SYSTEM_ITEM::SYSTEM>>( system, noRanges, true );

		CHECK( REG().get<System::Selection>( system ).atoms.count() == topology.getAtomCount() );
		CHECK( getSelectionState( { system, E_SYSTEM_ITEM::SYSTEM } ) == E_SELECTION_STATE::FULL );
	}
}

TEST_CASE( "VTX_APP - Selection - Clear handles this system, others and all systems", "[integration][selection]" )
{
	using namespace VTX;

	Fixture app;
	Entity	firstSystem	 = InvalidEntity;
	Entity	secondSystem = InvalidEntity;
	app.loadSystem();
	app.loadSystem();

	Registry & reg = App::REG();

	// Get the two loaded entities from view.
	{
		uint systemCount = 0;
		for ( const auto entity : reg.view<Topology>() )
		{
			if ( systemCount == 0 )
			{
				firstSystem = entity;
			}
			else if ( systemCount == 1 )
			{
				secondSystem = entity;
			}
			else
			{
				FAIL( "More than 2 systems loaded, test setup failure" );
			}
			systemCount++;
		}
		REQUIRE( systemCount == 2 );
		REQUIRE( firstSystem != InvalidEntity );
		REQUIRE( secondSystem != InvalidEntity );
	}

	ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( firstSystem, Index( 0 ) );
	ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( secondSystem, Index( 0 ) );

	CHECK( REG().get<System::Selection>( firstSystem ).atoms.count() == 1 );
	CHECK( REG().get<System::Selection>( secondSystem ).atoms.count() == 1 );

	ACTION().execute<Clear>( firstSystem, Clear::E_MODE::THIS );

	CHECK( REG().get<System::Selection>( firstSystem ).atoms.none() );
	CHECK( REG().get<System::Selection>( secondSystem ).atoms.count() == 1 );

	ACTION().execute<SetSelected<E_SYSTEM_ITEM::ATOM>>( firstSystem, Index( 0 ) );
	ACTION().execute<Clear>( firstSystem, Clear::E_MODE::OTHERS );

	CHECK( REG().get<System::Selection>( firstSystem ).atoms.count() == 1 );
	CHECK( REG().get<System::Selection>( secondSystem ).atoms.none() );

	ACTION().execute<Clear>();

	CHECK( REG().get<System::Selection>( firstSystem ).atoms.none() );
	CHECK( REG().get<System::Selection>( secondSystem ).atoms.none() );
}
