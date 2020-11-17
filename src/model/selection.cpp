#include "selection.hpp"
#include "event/event_manager.hpp"
#include "mvc/mvc_manager.hpp"
#include "tool/logger.hpp"

namespace VTX
{
	namespace Model
	{
		void Selection::update( const std::vector<ID> & p_selectedIds )
		{
			clear();
			for ( const ID & modelId : p_selectedIds )
			{
				ID::VTX_ID modelTypeId = MVC::MvcManager::get().getModelTypeID( modelId );
				if ( modelTypeId == ID::Model::MODEL_MOLECULE )
				{
					const Model::Molecule & model = MVC::MvcManager::get().getModel<Model::Molecule>( modelId );
					_selectMolecule( model );
				}
				else if ( modelTypeId == ID::Model::MODEL_CHAIN )
				{
					const Model::Chain & model = MVC::MvcManager::get().getModel<Model::Chain>( modelId );
					_selectChain( model );
				}
				else if ( modelTypeId == ID::Model::MODEL_RESIDUE )
				{
					const Model::Residue & model = MVC::MvcManager::get().getModel<Model::Residue>( modelId );
					_selectResidue( model );
				}
				else if ( modelTypeId == ID::Model::MODEL_ATOM )
				{
					const Model::Atom & model = MVC::MvcManager::get().getModel<Model::Atom>( modelId );
					_selectAtom( model );
				}
			}

			this->_notifyDataChanged();
			VTX_EVENT( new Event::VTXEventPtr( Event::Global::SELECTION_CHANGE, this ) );
		}

		void Selection::selectMolecule( const Molecule & p_molecule )
		{
			_selectMolecule( p_molecule );
			this->_notifyDataChanged();
		}

		void Selection::selectChain( const Chain & p_chain )
		{
			_selectChain( p_chain );
			this->_notifyDataChanged();
		}

		void Selection::selectResidue( const Residue & p_residue )
		{
			_selectResidue( p_residue );
			this->_notifyDataChanged();
		}

		void Selection::selectAtom( const Atom & p_atom )
		{
			_selectAtom( p_atom );
			this->_notifyDataChanged();
		}

		void Selection::unselectMolecule( const Molecule & p_molecule )
		{
			_unselectMolecule( p_molecule );
			this->_notifyDataChanged();
			VTX_EVENT( new Event::VTXEventPtr( Event::Global::SELECTION_CHANGE, this ) );
		}

		void Selection::unselectChain( const Chain & p_chain )
		{
			_unselectChain( p_chain );
			this->_notifyDataChanged();
			VTX_EVENT( new Event::VTXEventPtr( Event::Global::SELECTION_CHANGE, this ) );
		}

		void Selection::unselectResidue( const Residue & p_residue )
		{
			_unselectResidue( p_residue );
			this->_notifyDataChanged();
			VTX_EVENT( new Event::VTXEventPtr( Event::Global::SELECTION_CHANGE, this ) );
		}

		void Selection::unselectAtom( const Atom & p_atom )
		{
			_unselectAtom( p_atom );
			this->_notifyDataChanged();
			VTX_EVENT( new Event::VTXEventPtr( Event::Global::SELECTION_CHANGE, this ) );
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

		void Selection::clear() { _items.clear(); }

	} // namespace Model
} // namespace VTX
