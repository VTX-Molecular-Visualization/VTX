#include "selection.hpp"
#include "tool/logger.hpp"

namespace VTX
{
	namespace Model
	{
		void Selection::selectMolecule( const Model::Molecule & p_molecule )
		{
			Model::Model_ID id = p_molecule.getId();
			VTX_INFO( "selectMolecule" );
		}

		void Selection::selectChain( const Model::Chain & ) { VTX_INFO( "selectChain" ); }

		void Selection::selectResidue( const Model::Residue & ) { VTX_INFO( "selectResidue" ); }

		void Selection::selectAtom( const Model::Atom & ) { VTX_INFO( "selectAtom" ); }

		void Selection::unselectMolecule( const Model::Molecule & ) {}
		void Selection::unselectChain( const Model::Chain & ) {}
		void Selection::unselectResidue( const Model::Residue & ) {}
		void Selection::unselectAtom( const Model::Atom & ) {}

		void Selection::clear() {}

	} // namespace Model
} // namespace VTX
