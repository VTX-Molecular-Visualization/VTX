#include "selection.hpp"
#include "tool/logger.hpp"

namespace VTX
{
	namespace Model
	{
		void Selection::selectMolecule( const Molecule & p_molecule )
		{
			_addMolecule( p_molecule );
			_addMoleculeContent( p_molecule );
		}

		void Selection::selectChain( const Chain & p_chain )
		{
			_addMolecule( *p_chain.getMoleculePtr() );
			_addChain( p_chain );
			_addChainContent( p_chain );
		}

		void Selection::selectResidue( const Residue & p_residue )
		{
			_addMolecule( *p_residue.getMoleculePtr() );
			_addChain( *p_residue.getChainPtr() );
			_addResidue( p_residue );
			_addResidueContent( p_residue );
		}

		void Selection::selectAtom( const Atom & p_atom )
		{
			_addMolecule( *p_atom.getMoleculePtr() );
			_addChain( *p_atom.getChainPtr() );
			_addResidue( *p_atom.getResiduePtr() );
			_addAtom( p_atom );
		}

		void Selection::_addMolecule( const Molecule & p_molecule )
		{
			const ID & id = p_molecule.getId();

			if ( _items.find( id ) == _items.end() )
			{
				_items.emplace( id, std::map<ID, std::map<ID, std::vector<ID>>>() );
			}
		}

		void Selection::_addChain( const Chain & p_chain )
		{
			const ID & moleculeId = p_chain.getMoleculePtr()->getId();
			const ID & id		  = p_chain.getId();

			std::map<ID, std::map<ID, std::vector<ID>>> & chainMap = _items.at( moleculeId );

			if ( chainMap.find( id ) == chainMap.end() )
			{
				chainMap.emplace( id, std::map<ID, std::vector<ID>>() );
			}
		}

		void Selection::_addResidue( const Residue & p_residue )
		{
			const ID & moleculeId = p_residue.getMoleculePtr()->getId();
			const ID & chainId	  = p_residue.getChainPtr()->getId();
			const ID & id		  = p_residue.getId();

			std::map<ID, std::vector<ID>> & residueMap = _items.at( moleculeId ).at( chainId );

			if ( residueMap.find( id ) == residueMap.end() )
			{
				residueMap.emplace( id, std::vector<ID>() );
			}
		}

		void Selection::_addAtom( const Atom & p_atom )
		{
			const ID & moleculeId = p_atom.getMoleculePtr()->getId();
			const ID & chainId	  = p_atom.getChainPtr()->getId();
			const ID & residueId  = p_atom.getResiduePtr()->getId();
			const ID & id		  = p_atom.getId();

			std::vector<ID> & atomVector = _items.at( moleculeId ).at( chainId ).at( residueId );

			if ( std::find( atomVector.begin(), atomVector.end(), id ) == atomVector.end() )
			{
				atomVector.emplace_back( id );
			}
		}

		void Selection::_addMoleculeContent( const Molecule & p_molecule )
		{
			for ( uint i = 0; i < p_molecule.getChainCount(); ++i )
			{
				const Chain & chain = p_molecule.getChain( i );
				_addChain( chain );
				_addChainContent( chain );
			}
		}

		void Selection::_addChainContent( const Chain & p_chain )
		{
			for ( uint i = 0; i < p_chain.getResidueCount(); ++i )
			{
				const Residue & residue = p_chain.getMoleculePtr()->getResidue( p_chain.getIndexFirstResidue() + i );
				_addResidue( residue );
				_addResidueContent( residue );
			}
		}

		void Selection::_addResidueContent( const Residue & p_residue )
		{
			for ( uint i = 0; i < p_residue.getAtomCount(); ++i )
			{
				const Atom & atom = p_residue.getMoleculePtr()->getAtom( p_residue.getIndexFirstAtom() + i );
				_addAtom( atom );
			}
		}

		void Selection::unselectMolecule( const Molecule & p_molecule ) { _removeMolecule( p_molecule ); }

		void Selection::unselectChain( const Chain & p_chain ) { _removeChain( p_chain ); }

		void Selection::unselectResidue( const Residue & p_residue ) { _removeResidue( p_residue ); }

		void Selection::unselectAtom( const Atom & p_atom ) { _removeAtom( p_atom ); }

		void Selection::_removeMolecule( const Molecule & p_molecule )
		{
			const ID & id = p_molecule.getId();
			_items.erase( id );
		}

		void Selection::_removeChain( const Chain & p_chain )
		{
			const ID & moleculeId = p_chain.getMoleculePtr()->getId();
			const ID & id		  = p_chain.getId();

			_items.at( moleculeId ).erase( id );
			if ( _items.at( moleculeId ).size() == 0 )
			{
				_removeMolecule( *p_chain.getMoleculePtr() );
			}
		}

		void Selection::_removeResidue( const Residue & p_residue )
		{
			const ID & moleculeId = p_residue.getMoleculePtr()->getId();
			const ID & chainId	  = p_residue.getChainPtr()->getId();
			const ID & id		  = p_residue.getId();

			_items.at( moleculeId ).at( chainId ).erase( id );
			if ( _items.at( moleculeId ).at( chainId ).size() == 0 )
			{
				_removeChain( *p_residue.getChainPtr() );
			}
		}

		void Selection::_removeAtom( const Atom & p_atom )
		{
			const ID & moleculeId = p_atom.getMoleculePtr()->getId();
			const ID & chainId	  = p_atom.getChainPtr()->getId();
			const ID & residueId  = p_atom.getResiduePtr()->getId();
			const ID & id		  = p_atom.getId();

			std::vector<ID> & atomVector = _items.at( moleculeId ).at( chainId ).at( residueId );

			atomVector.erase( std::remove( atomVector.begin(), atomVector.end(), id ), atomVector.end() );
			if ( atomVector.size() == 0 )
			{
				_removeResidue( *p_atom.getResiduePtr() );
			}
		}

		void Selection::clear() { _items.clear(); }

	} // namespace Model
} // namespace VTX
