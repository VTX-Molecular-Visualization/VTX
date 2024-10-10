#include "util/app.hpp"
#include <app/application/scene.hpp>
#include <app/application/selection/molecule_data.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/component/scene/selectable.hpp>
#include <app/fixture.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>

TEST_CASE( "VTX_APP - Selection", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;
	try
	{
		VTX_INFO( "TEST VTX_APP - Selection" );

		using AssignmentType = Application::Selection::AssignmentType;

		App::Fixture app;

		Test::Util::App::loadMolecule( "8OIT.mmtf" );
		const Component::Chemistry::Molecule & mol1
			= SCENE().getComponentByName<Component::Chemistry::Molecule>( "8OIT" );
		const Component::Scene::Selectable & selectableMol1
			= SCENE().getComponentByName<Component::Scene::Selectable>( "8OIT" );

		Test::Util::App::loadMolecule( "1AGA.mmtf" );
		const Component::Chemistry::Molecule & mol2
			= SCENE().getComponentByName<Component::Chemistry::Molecule>( "1AGA" );
		const Component::Scene::Selectable & selectableMol2
			= SCENE().getComponentByName<Component::Scene::Selectable>( "1AGA" );
		VTX_INFO( "1AGA.mmtf loaded" );

		CHECK( CURRENT_SELECTION().isEmpty() );

		Application::Selection::SelectionData & molSelData1 = CURRENT_SELECTION().select( selectableMol1 );
		CHECK( CURRENT_SELECTION().isSelected( selectableMol1 ) );
		CHECK( !CURRENT_SELECTION().isSelected( selectableMol2 ) );
		CHECK( !CURRENT_SELECTION().areSelected( { &selectableMol1, &selectableMol2 } ) );
		CHECK( &CURRENT_SELECTION().getCurrentObject() == &selectableMol1 );

		Application::Selection::SelectionData & molSelData2 = CURRENT_SELECTION().select( mol2 );
		CHECK( !CURRENT_SELECTION().isSelected( selectableMol1 ) );
		CHECK( CURRENT_SELECTION().isSelected( selectableMol2 ) );
		CHECK( !CURRENT_SELECTION().areSelected( { &selectableMol1, &selectableMol2 } ) );
		CHECK( &CURRENT_SELECTION().getCurrentObject() == &selectableMol2 );

		CURRENT_SELECTION().select( selectableMol1, AssignmentType::APPEND );
		CHECK( CURRENT_SELECTION().isSelected( selectableMol1 ) );
		CHECK( CURRENT_SELECTION().isSelected( selectableMol2 ) );
		CHECK( CURRENT_SELECTION().areSelected( { &selectableMol1, &selectableMol2 } ) );
		CHECK( &CURRENT_SELECTION().getCurrentObject() == &selectableMol1 );

		CURRENT_SELECTION().clear();
		CHECK( CURRENT_SELECTION().isEmpty() );

		CURRENT_SELECTION().selectAll( { &selectableMol1, &selectableMol2 } );
		CHECK( CURRENT_SELECTION().areSelected( { &selectableMol1, &selectableMol2 } ) );
		CHECK( &CURRENT_SELECTION().getCurrentObject() == &selectableMol2 );

		CURRENT_SELECTION().clear();

		std::vector<const Component::Scene::Selectable *> selectables = { &selectableMol1, &selectableMol2 };
		CURRENT_SELECTION().selectAll( selectables );
		CHECK( CURRENT_SELECTION().areSelected( { &selectableMol1, &selectableMol2 } ) );
		CHECK( &CURRENT_SELECTION().getCurrentObject() == &selectableMol2 );

		CURRENT_SELECTION().clear();

		// std::vector<const Component::Chemistry::Molecule *> molecules = { &mol1, &mol2 };
		// CURRENT_SELECTION().selectAll( molecules );
		// CHECK( CURRENT_SELECTION().areSelected( { &selectableMol1, &selectableMol2 } ) );
	}
	catch ( const std::exception & e )
	{
		VTX_ERROR( "Exception: {}", e.what() );
	}
}

TEST_CASE( "VTX_APP - Selection - Molecules", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	using MoleculeData	 = Application::Selection::MoleculeData;
	using AssignmentType = Application::Selection::AssignmentType;
	using IndexRange	 = Util::Math::Range<size_t>;
	using AtomIndexRange = Util::Math::Range<atom_index_t>;

	App::Fixture app;

	Test::Util::App::loadMolecule( "1AGA.mmtf" );
	const Component::Chemistry::Molecule & mol1 = SCENE().getComponentByName<Component::Chemistry::Molecule>( "1AGA" );
	const Component::Scene::Selectable &   selectableMol1
		= SCENE().getComponentByName<Component::Scene::Selectable>( "1AGA" );

	MoleculeData & molSelData1 = CURRENT_SELECTION().select<MoleculeData>( selectableMol1 );
	CHECK( CURRENT_SELECTION().isSelected( selectableMol1 ) );
	CHECK( molSelData1.isFullySelected() );
	CHECK( molSelData1.getCurrentObjectType() == MoleculeData::CurrentObjectTypeEnum::Molecule );
	CHECK( &molSelData1.getCurrentObjectAsMolecule() == &mol1 );

	molSelData1.clear();
	molSelData1.selectFullChain( *mol1.getChain( 0 ) );
	CHECK( !molSelData1.isFullySelected() );
	CHECK( molSelData1.isChainSelected( 0 ) );
	CHECK( molSelData1.isChainFullySelected( 0 ) );
	CHECK( molSelData1.isResidueSelected( 0 ) );
	CHECK( molSelData1.isResidueFullySelected( 0 ) );
	CHECK( molSelData1.isAtomSelected( 0 ) );
	CHECK( molSelData1.getCurrentObjectType() == MoleculeData::CurrentObjectTypeEnum::Chain );
	CHECK( &molSelData1.getCurrentObjectAsChain() == mol1.getChain( 0 ) );

	molSelData1.unselectAtom( *mol1.getAtom( 0 ) );
	CHECK( molSelData1.isChainSelected( 0 ) );
	CHECK( !molSelData1.isChainFullySelected( 0 ) );
	CHECK( molSelData1.isResidueSelected( 0 ) );
	CHECK( !molSelData1.isResidueFullySelected( 0 ) );
	CHECK( !molSelData1.isAtomSelected( 0 ) );
	CHECK( molSelData1.getCurrentObjectType() == MoleculeData::CurrentObjectTypeEnum::Chain );
	CHECK( &molSelData1.getCurrentObjectAsChain() == mol1.getChain( 0 ) );

	molSelData1.selectAtom( *mol1.getAtom( 0 ) );
	CHECK( molSelData1.isChainSelected( 0 ) );
	CHECK( molSelData1.isChainFullySelected( 0 ) );
	CHECK( molSelData1.isResidueSelected( 0 ) );
	CHECK( molSelData1.isResidueFullySelected( 0 ) );
	CHECK( molSelData1.isAtomSelected( 0 ) );

	molSelData1.setCurrentObject( *mol1.getResidue( 0 ) );
	CHECK( molSelData1.getCurrentObjectType() == MoleculeData::CurrentObjectTypeEnum::Residue );
	CHECK( &molSelData1.getCurrentObjectAsResidue() == mol1.getResidue( 0 ) );

	molSelData1.unselectChain( *mol1.getChain( 0 ) );
	CHECK( !molSelData1.isChainSelected( 0 ) );
	CHECK( !molSelData1.isChainFullySelected( 0 ) );
	CHECK( !molSelData1.areResiduesSelected(
		IndexRange( mol1.getChain( 0 )->getIndexFirstResidue(), mol1.getChain( 0 )->getResidueCount() )
	) );
	CHECK( !molSelData1.areResiduesFullySelected(
		IndexRange( mol1.getChain( 0 )->getIndexFirstResidue(), mol1.getChain( 0 )->getResidueCount() )
	) );
	CHECK( !molSelData1.areAtomsSelected( AtomIndexRange::createFirstLast(
		mol1.getChain( 0 )->getIndexFirstAtom(), mol1.getChain( 0 )->getIndexLastAtom()
	) ) );

	CHECK( molSelData1.getCurrentObjectType() == MoleculeData::CurrentObjectTypeEnum::None );

	MoleculeData molData2 = MoleculeData( selectableMol1 );
	molSelData1.clear();
	molData2.clear();

	molData2.selectFullResidue( *mol1.getResidue( 0 ) );
	CHECK( !molSelData1.contains( molData2 ) );
	CHECK( molData2.contains( molSelData1 ) );

	molSelData1.selectAtom( *mol1.getAtom( 0 ) );
	CHECK( !molSelData1.contains( molData2 ) );
	CHECK( molData2.contains( molSelData1 ) );

	molSelData1.selectFullResidue( *mol1.getResidue( 0 ) );
	CHECK( molSelData1.contains( molData2 ) );
	CHECK( molData2.contains( molSelData1 ) );

	molSelData1.selectFullChain( *mol1.getChain( 0 ) );
	CHECK( molSelData1.contains( molData2 ) );
	CHECK( !molData2.contains( molSelData1 ) );

	molData2.selectFullChain( *mol1.getChain( 1 ) );
	CHECK( !molSelData1.contains( molData2 ) );
	CHECK( !molData2.contains( molSelData1 ) );
}

TEST_CASE( "VTX_APP - Selection - Benchmark", "[.][perfs]" )
{
	using namespace VTX;
	using namespace VTX::App;

	using AssignmentType = Application::Selection::AssignmentType;

	App::Fixture app;

	Test::Util::App::loadMolecule( "7Y7A.mmtf" );
	const Component::Chemistry::Molecule & mol1 = SCENE().getComponentByName<Component::Chemistry::Molecule>( "7Y7A" );
	const Component::Scene::Selectable &   selectableMol1
		= SCENE().getComponentByName<Component::Scene::Selectable>( "7Y7A" );

	BENCHMARK( "Select full molecule" ) { CURRENT_SELECTION().select( selectableMol1 ); };

	BENCHMARK( "Select molecule worst case" )
	{
		Application::Selection::MoleculeData & molData
			= CURRENT_SELECTION().select<Application::Selection::MoleculeData>( selectableMol1 );

		const atom_index_t atomCount = atom_index_t( mol1.getAtoms().size() );
		for ( atom_index_t i = 0; i < atomCount; i += 2 )
		{
			molData.selectAtom( *mol1.getAtom( i ) );
		}
	};
}
