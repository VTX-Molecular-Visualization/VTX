#include "util/app.hpp"
#include <app/application/scene.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/fixture.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE( "VTX_APP - Molecule - Remove", "[unit]" )
{
	using namespace VTX;
	using namespace VTX::App;
	using namespace VTX::App::Component::Chemistry;

	App::Fixture app;
	Test::Util::App::loadTestMolecule();

	App::Core::ECS::BaseEntity molEntity		 = SCENE().getItem( App::Test::Util::App::MOLECULE_TEST_NAME );
	Molecule &				   moleculeComponent = ECS_REGISTRY().getComponent<Molecule>( molEntity );

	atom_index_t expectedRemovedAtoms = 0;
	bool		 callbackValidated	  = 0;

	moleculeComponent.onAtomRemoved += [ & ]( const AtomIndexRangeList & p_atoms )
	{ callbackValidated = ( expectedRemovedAtoms == p_atoms.count() ); };

	// Remove first atom of residue
	{
		const atom_index_t	  atomIndex = moleculeComponent.getResidue( 0 )->getIndexFirstAtom();
		Atom * const		  atom		= moleculeComponent.getAtom( atomIndex );
		const Residue * const residue	= atom->getResiduePtr();

		expectedRemovedAtoms = 1;
		atom->remove();

		CHECK( callbackValidated );
		CHECK( moleculeComponent.getAtom( atomIndex ) == nullptr );
		CHECK( !moleculeComponent.getActiveAtoms().contains( atomIndex ) );
		CHECK( residue->getIndexFirstAtom() == ( atomIndex + 1 ) );
	}
	// Remove last atom of residue
	{
		const atom_index_t	  atomIndex = moleculeComponent.getResidue( 0 )->getIndexLastAtom();
		Atom * const		  atom		= moleculeComponent.getAtom( atomIndex );
		const Residue * const residue	= atom->getResiduePtr();

		atom->remove();

		CHECK( moleculeComponent.getAtom( atomIndex ) == nullptr );
		CHECK( !moleculeComponent.getActiveAtoms().contains( atomIndex ) );
		CHECK( residue->getIndexLastAtom() == ( atomIndex - 1 ) );
	}

	// Remove first residue
	{
		const size_t		 residueIndex	  = 0;
		Residue * const		 residue		  = moleculeComponent.getResidue( residueIndex );
		const AtomIndexRange residueAtomRange = residue->getAtomRange();
		const Chain * const	 chain			  = residue->getChainPtr();

		expectedRemovedAtoms = residueAtomRange.getCount();
		residue->remove();

		CHECK( callbackValidated == true );
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
		const Chain * const	 chain			  = moleculeComponent.getChain( 0 );
		const size_t		 residueIndex	  = chain->getIndexLastResidue();
		Residue * const		 residue		  = moleculeComponent.getResidue( residueIndex );
		const AtomIndexRange residueAtomRange = residue->getAtomRange();

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
		const size_t			chainIndex		  = 0;
		Chain * const			chain			  = moleculeComponent.getChain( chainIndex );
		const ResidueIndexRange chainResidueRange = chain->getResidueRange();
		const AtomIndexRange	chainAtomRange	  = chain->getAtomRange();

		expectedRemovedAtoms = chainAtomRange.getCount();
		chain->remove();

		CHECK( callbackValidated == true );
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
		const size_t	   defaultAtomSize = moleculeComponent.getAtoms().size();
		const atom_index_t atomIndex	   = atom_index_t( defaultAtomSize - 1 );
		Atom * const	   atom			   = moleculeComponent.getAtom( atomIndex );

		atom->remove();

		CHECK( moleculeComponent.getAtoms().size() == ( defaultAtomSize - 1 ) );
	}

	// Remove last residue
	{
		const size_t defaultResidueSize = moleculeComponent.getResidues().size();
		const size_t defaultAtomSize	= moleculeComponent.getAtoms().size();

		const size_t residueIndex = defaultResidueSize - 1;
		Residue &	 residue	  = *moleculeComponent.getResidue( residueIndex );

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

		const size_t chainIndex = defaultChainSize - 1;
		Chain &		 chain		= *moleculeComponent.getChain( chainIndex );

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
	using namespace VTX::App::Component::Chemistry;

	App::Fixture app;
	Test::Util::App::loadTestMolecule();

	App::Core::ECS::BaseEntity molEntity		 = SCENE().getItem( App::Test::Util::App::MOLECULE_TEST_NAME );
	Molecule &				   moleculeComponent = ECS_REGISTRY().getComponent<Molecule>( molEntity );

	atom_index_t expectedModifiedAtomCount = 0;
	bool		 callbackValidated		   = 0;

	moleculeComponent.onVisibilityChange +=
		[ & ]( const AtomIndexRangeList & p_atoms, const App::Core::VISIBILITY_APPLY_MODE p_applyMode )
	{ callbackValidated = ( expectedModifiedAtomCount == p_atoms.count() ); };

	// Atom visibility
	{
		const atom_index_t atomIndex = 0;
		Atom &			   atom		 = *( moleculeComponent.getAtom( atomIndex ) );
		const Residue &	   residue	 = *( atom.getConstResiduePtr() );

		expectedModifiedAtomCount = 1;

		atom.setVisible( false );
		CHECK( callbackValidated );
		CHECK( atom.isVisible() == false );
		CHECK( residue.isVisible() );

		atom.setVisible( true );
		CHECK( callbackValidated );
		CHECK( atom.isVisible() == true );
		CHECK( residue.isVisible() );
	}

	// Residues visibility
	{
		Residue &			 residue   = *( moleculeComponent.getResidue( 0 ) );
		const AtomIndexRange atomRange = residue.getAtomRange();

		expectedModifiedAtomCount = atomRange.getCount();

		// From residue
		{
			residue.setVisible( false );
			CHECK( callbackValidated );
			CHECK( moleculeComponent.getAtom( atomRange.getFirst() )->isVisible() == false );
			CHECK( moleculeComponent.getAtom( atomRange.getLast() )->isVisible() == false );
			CHECK( moleculeComponent.getAtom( atomRange.getLast() + 1 )->isVisible() == true );
			CHECK( residue.isVisible() == false );

			residue.setVisible( true );
			CHECK( callbackValidated );
			CHECK( moleculeComponent.getAtom( atomRange.getFirst() )->isVisible() == true );
			CHECK( moleculeComponent.getAtom( atomRange.getLast() )->isVisible() == true );
			CHECK( residue.isVisible() == true );
		}

		// From atoms
		{
			moleculeComponent.setVisible( atomRange, false );
			CHECK( callbackValidated );
			CHECK( moleculeComponent.getAtom( atomRange.getFirst() )->isVisible() == false );
			CHECK( moleculeComponent.getAtom( atomRange.getLast() )->isVisible() == false );
			CHECK( moleculeComponent.getAtom( atomRange.getLast() + 1 )->isVisible() == true );
			CHECK( residue.isVisible() == false );

			moleculeComponent.setVisible( atomRange, true );
			CHECK( callbackValidated );
			CHECK( moleculeComponent.getAtom( atomRange.getFirst() )->isVisible() == true );
			CHECK( moleculeComponent.getAtom( atomRange.getLast() )->isVisible() == true );
			CHECK( residue.isVisible() == true );
		}
	}

	// Chains visibility
	{
		Chain &					chain		 = *( moleculeComponent.getChain( 0 ) );
		Residue &				residue		 = *( moleculeComponent.getResidue( 0 ) );
		const AtomIndexRange	atomRange	 = chain.getAtomRange();
		const ResidueIndexRange residueRange = chain.getResidueRange();

		expectedModifiedAtomCount = atomRange.getCount();

		// From chain
		{
			chain.setVisible( false );
			CHECK( callbackValidated );
			CHECK( moleculeComponent.getAtom( atomRange.getFirst() )->isVisible() == false );
			CHECK( moleculeComponent.getAtom( atomRange.getLast() )->isVisible() == false );
			CHECK( moleculeComponent.getAtom( atomRange.getLast() + 1 )->isVisible() == true );
			CHECK( moleculeComponent.getResidue( residueRange.getFirst() )->isVisible() == false );
			CHECK( moleculeComponent.getResidue( residueRange.getLast() )->isVisible() == false );
			CHECK( moleculeComponent.getResidue( residueRange.getLast() + 1 )->isVisible() == true );
			CHECK( chain.isVisible() == false );

			chain.setVisible( true );
			CHECK( callbackValidated );
			CHECK( moleculeComponent.getAtom( atomRange.getFirst() )->isVisible() == true );
			CHECK( moleculeComponent.getAtom( atomRange.getLast() )->isVisible() == true );
			CHECK( moleculeComponent.getResidue( residueRange.getFirst() )->isVisible() == true );
			CHECK( moleculeComponent.getResidue( residueRange.getLast() )->isVisible() == true );
			CHECK( chain.isVisible() == true );
		}

		// From residues
		{
			for ( Residue & residue : chain.residues() )
				residue.setVisible( false );

			CHECK( moleculeComponent.getAtom( atomRange.getFirst() )->isVisible() == false );
			CHECK( moleculeComponent.getAtom( atomRange.getLast() )->isVisible() == false );
			CHECK( moleculeComponent.getAtom( atomRange.getLast() + 1 )->isVisible() == true );
			CHECK( moleculeComponent.getResidue( residueRange.getFirst() )->isVisible() == false );
			CHECK( moleculeComponent.getResidue( residueRange.getLast() )->isVisible() == false );
			CHECK( moleculeComponent.getResidue( residueRange.getLast() + 1 )->isVisible() == true );
			CHECK( chain.isVisible() == false );

			for ( Residue & residue : chain.residues() )
				residue.setVisible( true );
			CHECK( moleculeComponent.getAtom( atomRange.getFirst() )->isVisible() == true );
			CHECK( moleculeComponent.getAtom( atomRange.getLast() )->isVisible() == true );
			CHECK( moleculeComponent.getResidue( residueRange.getFirst() )->isVisible() == true );
			CHECK( moleculeComponent.getResidue( residueRange.getLast() )->isVisible() == true );
			CHECK( chain.isVisible() == true );
		}

		// From atoms
		{
			moleculeComponent.setVisible( atomRange, false );

			CHECK( callbackValidated );
			CHECK( moleculeComponent.getAtom( atomRange.getFirst() )->isVisible() == false );
			CHECK( moleculeComponent.getAtom( atomRange.getLast() )->isVisible() == false );
			CHECK( moleculeComponent.getAtom( atomRange.getLast() + 1 )->isVisible() == true );
			CHECK( moleculeComponent.getResidue( residueRange.getFirst() )->isVisible() == false );
			CHECK( moleculeComponent.getResidue( residueRange.getLast() )->isVisible() == false );
			CHECK( moleculeComponent.getResidue( residueRange.getLast() + 1 )->isVisible() == true );
			CHECK( chain.isVisible() == false );

			moleculeComponent.setVisible( atomRange, true );
			CHECK( callbackValidated );
			CHECK( moleculeComponent.getAtom( atomRange.getFirst() )->isVisible() == true );
			CHECK( moleculeComponent.getAtom( atomRange.getLast() )->isVisible() == true );
			CHECK( moleculeComponent.getResidue( residueRange.getFirst() )->isVisible() == true );
			CHECK( moleculeComponent.getResidue( residueRange.getLast() )->isVisible() == true );
			CHECK( chain.isVisible() == true );
		}
	}
};
