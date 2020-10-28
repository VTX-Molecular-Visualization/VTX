#include "selection.hpp"

namespace VTX
{
	namespace Model
	{
		void Selection::selectMolecule( Model::Molecule * const ) {}
		void Selection::unselectMolecule( Model::Molecule * const ) {}
		void Selection::selectChain( Model::Chain * const ) {}
		void Selection::unselectChain( Model::Chain * const ) {}
		void Selection::selectResidue( Model::Residue * const ) {}
		void Selection::unselectResidue( Model::Residue * const ) {}
		void Selection::selectAtom( Model::Atom * const ) {}
		void Selection::unselectAtom( Model::Atom * const ) {}
		void Selection::clear() {}

	} // namespace Model
} // namespace VTX
