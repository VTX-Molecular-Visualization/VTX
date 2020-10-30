#include "selection.hpp"
#include "tool/logger.hpp"

namespace VTX
{
	namespace Model
	{
		void Selection::selectMolecule( const Model::Molecule & p_molecule ) {}

		void Selection::selectChain( const Model::Chain & p_chain ) {}

		void Selection::selectResidue( const Model::Residue & p_residue ) {}

		void Selection::selectAtom( const Model::Atom & p_atom ) {}

		void Selection::_addMolecule( const Model::Molecule & p_molecule )
		{
			const Model::Model_ID & id = p_molecule.getId();
			if ( _items.find( id ) == _items.end() )
			{
				_items.try_emplace( id, std::map<uint, std::map<uint, std::vector<uint>>>() );
			}
		}

		void Selection::_addChain( const Model::Chain & p_chain ) {}

		void Selection::_addResidue( const Model::Residue & p_residue ) {}

		void Selection::_addMoleculeContent( const Model::Molecule & p_molecule ) {}

		void Selection::_addChainContent( const Model::Chain & p_chain ) {}

		void Selection::_addResidueContent( const Model::Residue & p_residue ) {}

		void Selection::unselectMolecule( const Model::Molecule & p_molecule ) {}

		void Selection::unselectChain( const Model::Chain & p_chain ) {}

		void Selection::unselectResidue( const Model::Residue & p_residue ) {}

		void Selection::unselectAtom( const Model::Atom & p_atom ) {}

		void Selection::clear() {}

	} // namespace Model
} // namespace VTX
