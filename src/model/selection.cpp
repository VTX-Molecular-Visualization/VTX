#include "selection.hpp"
#include "event/event_manager.hpp"
#include "mvc/mvc_manager.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"

namespace VTX
{
	namespace Model
	{
		void Selection::selectMolecule( Molecule & p_molecule )
		{
			Tool::Chrono chrono = Tool::Chrono();
			chrono.start();
			_selectMolecule( p_molecule );
			chrono.stop();
			VTX_INFO( "Selection time: " + std::to_string( chrono.elapsedTime() ) );
			p_molecule.refreshSelection( &_items[ p_molecule.getId() ] );
			_notifyDataChanged();
		}
		void Selection::selectMolecules( const std::vector<Molecule *> & p_molecules )
		{
			for ( const auto it : p_molecules )
			{
				_selectMolecule( *it );
				it->refreshSelection( &_items[ it->getId() ] );
			}
			_notifyDataChanged();
		}

		void Selection::selectChain( Chain & p_chain )
		{
			_selectChain( p_chain );
			_refreshMoleculeSelection( p_chain.getMoleculePtr() );
			_notifyDataChanged();
		}
		void Selection::selectChains( const std::vector<Chain *> & p_chains )
		{
			if ( p_chains.size() == 0 )
				return;

			for ( auto it : p_chains )
				_selectChain( *it );

			_refreshMoleculeSelection( p_chains[ 0 ]->getMoleculePtr() );
			_notifyDataChanged();
		}

		void Selection::selectResidue( Residue & p_residue )
		{
			_selectResidue( p_residue );
			_refreshMoleculeSelection( p_residue.getMoleculePtr() );
			_notifyDataChanged();
		}
		void Selection::selectResidues( const std::vector<Residue *> & p_residues )
		{
			if ( p_residues.size() == 0 )
				return;

			for ( const auto it : p_residues )
				_selectResidue( *it );

			_refreshMoleculeSelection( p_residues[ 0 ]->getMoleculePtr() );
			_notifyDataChanged();
		}

		void Selection::selectAtom( Atom & p_atom )
		{
			_selectAtom( p_atom );
			_refreshMoleculeSelection( p_atom.getMoleculePtr() );
			_notifyDataChanged();
		}
		void Selection::selectAtoms( const std::vector<Atom *> & p_atoms )
		{
			if ( p_atoms.size() == 0 )
				return;

			for ( const auto it : p_atoms )
				_selectAtom( *it );

			_refreshMoleculeSelection( p_atoms[ 0 ]->getMoleculePtr() );
			_notifyDataChanged();
		}

		void Selection::unselectMolecule( Molecule & p_molecule )
		{
			_unselectMolecule( p_molecule );
			_refreshMoleculeSelection( &p_molecule );
			_notifyDataChanged();
		}
		void Selection::unselectMolecules( const std::vector<Molecule *> & p_molecules )
		{
			if ( p_molecules.size() == 0 )
				return;

			for ( const auto it : p_molecules )
			{
				_unselectMolecule( *it );
				_refreshMoleculeSelection( it );
			}
			_notifyDataChanged();
		}

		void Selection::unselectChain( Chain & p_chain )
		{
			_unselectChain( p_chain );
			_refreshMoleculeSelection( p_chain.getMoleculePtr() );
			_notifyDataChanged();
		}
		void Selection::unselectChains( const std::vector<Chain *> & p_chains )
		{
			if ( p_chains.size() == 0 )
				return;

			for ( const auto it : p_chains )
				_unselectChain( *it );

			_refreshMoleculeSelection( p_chains[ 0 ]->getMoleculePtr() );
			_notifyDataChanged();
		}

		void Selection::unselectResidue( Residue & p_residue )
		{
			_unselectResidue( p_residue );
			_refreshMoleculeSelection( p_residue.getMoleculePtr() );
			_notifyDataChanged();
		}
		void Selection::unselectResidues( const std::vector<Residue *> & p_residues )
		{
			if ( p_residues.size() == 0 )
				return;

			for ( const auto it : p_residues )
				_unselectResidue( *it );

			_refreshMoleculeSelection( p_residues[ 0 ]->getMoleculePtr() );
			_notifyDataChanged();
		}

		void Selection::unselectAtom( Atom & p_atom )
		{
			_unselectAtom( p_atom );
			_refreshMoleculeSelection( p_atom.getMoleculePtr() );
			_notifyDataChanged();
		}
		void Selection::unselectAtoms( const std::vector<Atom *> & p_atoms )
		{
			if ( p_atoms.size() == 0 )
				return;

			for ( const auto it : p_atoms )
				_unselectAtom( *it );

			_refreshMoleculeSelection( p_atoms[ 0 ]->getMoleculePtr() );
			_notifyDataChanged();
		}

		void Selection::_selectMolecule( const Molecule & p_molecule )
		{
			_addMolecule( p_molecule );
			_addMoleculeContent( p_molecule );
		}

		void Selection::_selectChain( const Chain & p_chain )
		{
			_addMolecule( *p_chain.getMoleculePtr() );
			_addChain( p_chain );
			_addChainContent( p_chain );
		}

		void Selection::_selectResidue( const Residue & p_residue )
		{
			_addMolecule( *p_residue.getMoleculePtr() );
			_addChain( *p_residue.getChainPtr() );
			_addResidue( p_residue );
			_addResidueContent( p_residue );
		}

		void Selection::_selectAtom( const Atom & p_atom )
		{
			_addMolecule( *p_atom.getMoleculePtr() );
			_addChain( *p_atom.getChainPtr() );
			_addResidue( *p_atom.getResiduePtr() );
			_addAtom( p_atom );
		}

		void Selection::_unselectMolecule( const Molecule & p_molecule ) { _removeMolecule( p_molecule ); }

		void Selection::_unselectChain( const Chain & p_chain ) { _removeChain( p_chain ); }

		void Selection::_unselectResidue( const Residue & p_residue ) { _removeResidue( p_residue ); }

		void Selection::_unselectAtom( const Atom & p_atom ) { _removeAtom( p_atom ); }

		void Selection::_addMolecule( const Molecule & p_molecule )
		{
			const ID & id = p_molecule.getId();

			if ( _items.find( id ) == _items.end() )
			{
				_items.emplace( id, MapChainIds() );
			}
		}

		void Selection::_addChain( const Chain & p_chain )
		{
			const ID &	 moleculeId = p_chain.getMoleculePtr()->getId();
			const uint & index		= p_chain.getIndex();

			MapChainIds & chainMap = _items.at( moleculeId );

			if ( chainMap.find( index ) == chainMap.end() )
			{
				chainMap.emplace( index, MapResidueIds() );
			}
		}

		void Selection::_addResidue( const Residue & p_residue )
		{
			const ID &	 moleculeId = p_residue.getMoleculePtr()->getId();
			const uint & chainIndex = p_residue.getChainPtr()->getIndex();
			const uint & index		= p_residue.getIndex();

			MapResidueIds & residueMap = _items.at( moleculeId ).at( chainIndex );

			if ( residueMap.find( index ) == residueMap.end() )
			{
				residueMap.emplace( index, VecAtomIds() );
			}
		}

		void Selection::_addAtom( const Atom & p_atom )
		{
			const ID &	 moleculeId	  = p_atom.getMoleculePtr()->getId();
			const uint & chainIndex	  = p_atom.getChainPtr()->getIndex();
			const uint & residueIndex = p_atom.getResiduePtr()->getIndex();
			const uint & index		  = p_atom.getIndex();

			VecAtomIds & atomVector = _items.at( moleculeId ).at( chainIndex ).at( residueIndex );

			if ( std::find( atomVector.begin(), atomVector.end(), index ) == atomVector.end() )
			{
				atomVector.emplace_back( index );
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

		void Selection::_removeMolecule( const Molecule & p_molecule )
		{
			const ID & id = p_molecule.getId();
			_items.erase( id );
		}

		void Selection::_removeChain( const Chain & p_chain )
		{
			const ID & moleculeId = p_chain.getMoleculePtr()->getId();
			const ID & index	  = p_chain.getIndex();

			_items.at( moleculeId ).erase( index );
			if ( _items.at( moleculeId ).size() == 0 )
			{
				_removeMolecule( *p_chain.getMoleculePtr() );
			}
		}

		void Selection::_removeResidue( const Residue & p_residue )
		{
			const ID & moleculeId = p_residue.getMoleculePtr()->getId();
			const ID & chainIndex = p_residue.getChainPtr()->getIndex();
			const ID & index	  = p_residue.getIndex();

			_items.at( moleculeId ).at( chainIndex ).erase( index );
			if ( _items.at( moleculeId ).at( chainIndex ).size() == 0 )
			{
				_removeChain( *p_residue.getChainPtr() );
			}
		}

		void Selection::_removeAtom( const Atom & p_atom )
		{
			const ID & moleculeId	= p_atom.getMoleculePtr()->getId();
			const ID & chainIndex	= p_atom.getChainPtr()->getIndex();
			const ID & residueIndex = p_atom.getResiduePtr()->getIndex();
			const ID & index		= p_atom.getIndex();

			VecAtomIds & atomVector = _items.at( moleculeId ).at( chainIndex ).at( residueIndex );

			atomVector.erase( std::remove( atomVector.begin(), atomVector.end(), index ), atomVector.end() );
			if ( atomVector.size() == 0 )
			{
				_removeResidue( *p_atom.getResiduePtr() );
			}
		}

		void Selection::_refreshMoleculeSelection( Molecule * const p_molecule )
		{
			if ( _items.find( p_molecule->getId() ) != _items.end() )
			{
				p_molecule->refreshSelection( &_items[ p_molecule->getId() ] );
			}
			else
			{
				p_molecule->refreshSelection( nullptr );
			}
		}

		void Selection::clear()
		{
			_items.clear();
			this->_notifyDataChanged();
		}

		void Selection::receiveEvent( const Event::VTXEvent & p_event )
		{
			if ( p_event.name == Event::MOLECULE_REMOVED )
			{
				const Event::VTXEventPtr<Model::Molecule> & castedEvent = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
				unselectMolecule( *castedEvent.ptr );
			}
		}

	} // namespace Model
} // namespace VTX
