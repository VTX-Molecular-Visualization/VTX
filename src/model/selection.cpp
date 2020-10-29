#include "selection.hpp"

namespace VTX
{
	namespace Model
	{
		void Selection::selectMolecule( const Model::Molecule & p_molecule ) { Model::Model_ID id = p_molecule.getId(); }

		void Selection::selectChain( const Model::Chain & ) {}

		void Selection::selectResidue( const Model::Residue & ) {}

		void Selection::selectAtom( const Model::Atom & ) {}

		void Selection::unselectMolecule( const Model::Molecule & ) {}
		void Selection::unselectChain( const Model::Chain & ) {}
		void Selection::unselectResidue( const Model::Residue & ) {}
		void Selection::unselectAtom( const Model::Atom & ) {}

		void Selection::clear() {}

	} // namespace Model
} // namespace VTX
