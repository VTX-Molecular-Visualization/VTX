#include "util/app.hpp"
#include <app/application/ecs/registry_manager.hpp>
#include <app/application/scene.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "VTX_APP - Molecule - Remove", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	Test::Util::App::initApp();
	Test::Util::App::loadTestMolecule();

	App::Core::ECS::BaseEntity			  molEntity = SCENE().getItem( App::Test::Util::App::MOLECULE_TEST_NAME );
	App::Component::Chemistry::Molecule & moleculeComponent
		= MAIN_REGISTRY().getComponent<App::Component::Chemistry::Molecule>( molEntity );

	// Remove first atom of residue
	{
		const atom_index_t							atomIndex = moleculeComponent.getResidue( 0 )->getIndexFirstAtom();
		Component::Chemistry::Atom * const			atom	  = moleculeComponent.getAtom( atomIndex );
		const Component::Chemistry::Residue * const residue	  = atom->getResiduePtr();

		atom->remove();

		CHECK( moleculeComponent.getAtom( atomIndex ) == nullptr );
		CHECK( !moleculeComponent.getActiveAtoms().contains( atomIndex ) );
		CHECK( residue->getIndexFirstAtom() == ( atomIndex + 1 ) );
	}
	// Remove last atom of residue
	{
		const atom_index_t							atomIndex = moleculeComponent.getResidue( 0 )->getIndexLastAtom();
		Component::Chemistry::Atom * const			atom	  = moleculeComponent.getAtom( atomIndex );
		const Component::Chemistry::Residue * const residue	  = atom->getResiduePtr();

		atom->remove();

		CHECK( moleculeComponent.getAtom( atomIndex ) == nullptr );
		CHECK( !moleculeComponent.getActiveAtoms().contains( atomIndex ) );
		CHECK( residue->getIndexLastAtom() == ( atomIndex - 1 ) );
	}

	// Remove first residue
	{
		const size_t							   residueIndex = 0;
		Component::Chemistry::Residue * const	   residue		= moleculeComponent.getResidue( residueIndex );
		const Component::Chemistry::AtomIndexRange residueAtomRange
			= Component::Chemistry::AtomIndexRange( residue->getIndexFirstAtom(), residue->getAtomCount() );
		const Component::Chemistry::Chain * const chain = residue->getChainPtr();

		residue->remove();

		CHECK( moleculeComponent.getResidue( residueIndex ) == nullptr );
		CHECK( !moleculeComponent.getActiveAtoms().contains( residueAtomRange ) );

		for ( atom_index_t i = residueAtomRange.getFirst(); i <= residueAtomRange.getLast(); i++ )
		{
			CHECK( moleculeComponent.getAtom( i ) == nullptr );
		}
		CHECK( chain->getIndexFirstResidue() == ( residueIndex + 1 ) );
	}

	// Remove last residue of chain
	{
		const Component::Chemistry::Chain * const  chain		= moleculeComponent.getChain( 0 );
		const size_t							   residueIndex = chain->getIndexLastResidue();
		Component::Chemistry::Residue * const	   residue		= moleculeComponent.getResidue( residueIndex );
		const Component::Chemistry::AtomIndexRange residueAtomRange
			= Component::Chemistry::AtomIndexRange( residue->getIndexFirstAtom(), residue->getAtomCount() );

		residue->remove();

		CHECK( moleculeComponent.getResidue( residueIndex ) == nullptr );
		CHECK( !moleculeComponent.getActiveAtoms().contains( residueAtomRange ) );

		for ( atom_index_t i = residueAtomRange.getFirst(); i <= residueAtomRange.getLast(); i++ )
		{
			CHECK( moleculeComponent.getAtom( i ) == nullptr );
		}
		CHECK( chain->getIndexLastResidue() == ( residueIndex - 1 ) );
	}

	// Remove first chain
	{
		const size_t						chainIndex = 0;
		Component::Chemistry::Chain * const chain	   = moleculeComponent.getChain( chainIndex );
		const Util::Math::Range<size_t>		chainResidueRange
			= Util::Math::Range<size_t>( chain->getIndexFirstResidue(), chain->getResidueCount() );
		const Component::Chemistry::AtomIndexRange chainAtomRange
			= Component::Chemistry::AtomIndexRange::createFirstLast(
				chain->getIndexFirstAtom(), chain->getIndexLastAtom()
			);

		chain->remove();

		CHECK( moleculeComponent.getChain( chainIndex ) == nullptr );
		CHECK( !moleculeComponent.getActiveAtoms().contains( chainAtomRange ) );

		for ( atom_index_t i = chainAtomRange.getFirst(); i <= chainAtomRange.getLast(); i++ )
		{
			CHECK( moleculeComponent.getAtom( i ) == nullptr );
		}

		for ( size_t i = chainResidueRange.getFirst(); i <= chainResidueRange.getLast(); i++ )
		{
			CHECK( moleculeComponent.getResidue( i ) == nullptr );
		}
	}

	// Remove last atom
	{
		const size_t					   defaultAtomSize = moleculeComponent.getAtoms().size();
		const atom_index_t				   atomIndex	   = atom_index_t( defaultAtomSize - 1 );
		Component::Chemistry::Atom * const atom			   = moleculeComponent.getAtom( atomIndex );

		atom->remove();

		CHECK( moleculeComponent.getAtoms().size() == ( defaultAtomSize - 1 ) );
	}

	// Remove last residue
	{
		const size_t defaultResidueSize = moleculeComponent.getResidues().size();
		const size_t defaultAtomSize	= moleculeComponent.getAtoms().size();

		const size_t					residueIndex = defaultResidueSize - 1;
		Component::Chemistry::Residue & residue		 = *moleculeComponent.getResidue( residueIndex );

		const atom_index_t residueAtomCount = residue.getAtomCount();

		residue.remove();

		CHECK( moleculeComponent.getAtoms().size() == ( defaultAtomSize - residueAtomCount ) );
		CHECK( moleculeComponent.getResidues().size() == ( defaultResidueSize - 1 ) );
	}

	// Remove last chain
	{
		const size_t defaultRealChainCount	 = moleculeComponent.getRealChainCount();
		const size_t defaultRealResidueCount = moleculeComponent.getRealResidueCount();
		const size_t defaultRealAtomCount	 = moleculeComponent.getRealAtomCount();

		const size_t defaultChainSize	= moleculeComponent.getChains().size();
		const size_t defaultResidueSize = moleculeComponent.getResidues().size();
		const size_t defaultAtomSize	= moleculeComponent.getAtoms().size();

		const size_t				  chainIndex = defaultChainSize - 1;
		Component::Chemistry::Chain & chain		 = *moleculeComponent.getChain( chainIndex );

		const size_t	   chainResidueCount = chain.getResidueCount();
		const atom_index_t chainAtomCount	 = chain.getIndexLastAtom() - chain.getIndexFirstAtom() + 1;

		chain.remove();

		CHECK( moleculeComponent.getChains().size() == ( defaultChainSize - 1 ) );
		CHECK( moleculeComponent.getResidues().size() == ( defaultResidueSize - chainResidueCount ) );
		CHECK( moleculeComponent.getAtoms().size() == ( defaultAtomSize - chainAtomCount ) );

		CHECK( moleculeComponent.getRealChainCount() == ( defaultRealChainCount - 1 ) );
		CHECK( moleculeComponent.getRealResidueCount() == ( defaultRealResidueCount - chainResidueCount ) );
		CHECK( moleculeComponent.getRealAtomCount() == ( defaultRealAtomCount - chainAtomCount ) );
	}
};

TEST_CASE( "VTX_APP - Molecule - Visibility", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;

	Test::Util::App::initApp();
	Test::Util::App::loadTestMolecule();

	App::Core::ECS::BaseEntity			  molEntity = SCENE().getItem( App::Test::Util::App::MOLECULE_TEST_NAME );
	App::Component::Chemistry::Molecule & moleculeComponent
		= MAIN_REGISTRY().getComponent<App::Component::Chemistry::Molecule>( molEntity );
};
