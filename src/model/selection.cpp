#include "selection.hpp"
#include "event/event_manager.hpp"
#include "mvc/mvc_manager.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"

namespace VTX
{
	namespace Model
	{
		void Selection::selectMolecule( Molecule & p_molecule, const bool p_appendToSelection )
		{
			if ( !p_appendToSelection )
				_clearWithoutNotify();

			Tool::Chrono chrono = Tool::Chrono();
			chrono.start();
			_selectMolecule( p_molecule );
			chrono.stop();
			VTX_INFO( "Selection time: " + std::to_string( chrono.elapsedTime() ) );
			p_molecule.refreshSelection( &_items[ p_molecule.getId() ] );
			_notifyDataChanged();
		}
		void Selection::selectMolecules( const std::vector<Molecule *> & p_molecules, const bool p_appendToSelection )
		{
			if ( !p_appendToSelection )
				_clearWithoutNotify();

			for ( const auto it : p_molecules )
			{
				_selectMolecule( *it );
				it->refreshSelection( &_items[ it->getId() ] );
			}
			_notifyDataChanged();
		}

		void Selection::selectChain( Chain & p_chain, const bool p_appendToSelection )
		{
			if ( !p_appendToSelection )
				_clearWithoutNotify();

			_selectChain( p_chain );
			_refreshMoleculeSelection( p_chain.getMoleculePtr() );
			_notifyDataChanged();
		}
		void Selection::selectChains( const std::vector<Chain *> & p_chains, const bool p_appendToSelection )
		{
			if ( !p_appendToSelection )
				_clearWithoutNotify();

			if ( p_chains.size() == 0 )
				return;

			for ( auto it : p_chains )
				_selectChain( *it );

			_refreshMoleculeSelection( p_chains[ 0 ]->getMoleculePtr() );
			_notifyDataChanged();
		}

		void Selection::selectResidue( Residue & p_residue, const bool p_appendToSelection )
		{
			if ( !p_appendToSelection )
				_clearWithoutNotify();

			_selectResidue( p_residue );
			_refreshMoleculeSelection( p_residue.getMoleculePtr() );
			_notifyDataChanged();
		}
		void Selection::selectResidues( const std::vector<Residue *> & p_residues, const bool p_appendToSelection )
		{
			if ( !p_appendToSelection )
				_clearWithoutNotify();

			if ( p_residues.size() == 0 )
				return;

			for ( const auto it : p_residues )
				_selectResidue( *it );

			_refreshMoleculeSelection( p_residues[ 0 ]->getMoleculePtr() );
			_notifyDataChanged();
		}

		void Selection::selectAtom( Atom & p_atom, const bool p_appendToSelection )
		{
			if ( !p_appendToSelection )
				_clearWithoutNotify();

			_selectAtom( p_atom );
			_refreshMoleculeSelection( p_atom.getMoleculePtr() );
			_notifyDataChanged();
		}
		void Selection::selectAtoms( const std::vector<Atom *> & p_atoms, const bool p_appendToSelection )
		{
			if ( !p_appendToSelection )
				_clearWithoutNotify();

			if ( p_atoms.size() == 0 )
				return;

			for ( const auto it : p_atoms )
				_selectAtom( *it );

			_refreshMoleculeSelection( p_atoms[ 0 ]->getMoleculePtr() );
			_notifyDataChanged();
		}

		void Selection::selectRepresentation( Representation::InstantiatedRepresentation & p_representation,
											  const bool								   p_appendToSelection )
		{
			if ( !p_appendToSelection )
				_clearWithoutNotify();

			_selectRepresentation( p_representation );
			_notifyDataChanged();
		}
		void Selection::selectRepresentations(
			const std::vector<Representation::InstantiatedRepresentation *> & p_representations,
			const bool														  p_appendToSelection )
		{
			if ( !p_appendToSelection )
				_clearWithoutNotify();

			for ( const auto it : p_representations )
				_selectRepresentation( *it );

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
		void Selection::unselectMoleculesWithCheck( const std::vector<Molecule *> & p_molecules )
		{
			for ( const auto it : p_molecules )
			{
				Model::Molecule & molecule = *it;
				if ( isMoleculeSelected( molecule ) )
				{
					_unselectMolecule( molecule );
					molecule.refreshSelection( &_items[ molecule.getId() ] );
				}
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
		void Selection::unselectChainsWithCheck( const std::vector<Chain *> & p_chains )
		{
			for ( const auto it : p_chains )
			{
				if ( isChainSelected( *it ) )
					_unselectChain( *it );
			}

			p_chains[ 0 ]->getMoleculePtr()->refreshSelection( &_items[ p_chains[ 0 ]->getMoleculePtr()->getId() ] );
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
		void Selection::unselectResiduesWithCheck( const std::vector<Residue *> & p_residues )
		{
			for ( const auto it : p_residues )
			{
				if ( isResidueSelected( *it ) )
					_unselectResidue( *it );
			}
			p_residues[ 0 ]->getMoleculePtr()->refreshSelection(
				&_items[ p_residues[ 0 ]->getMoleculePtr()->getId() ] );
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
		void Selection::unselectAtomsWithCheck( const std::vector<Atom *> & p_atoms )
		{
			for ( const auto it : p_atoms )
			{
				if ( isAtomSelected( *it ) )
					_unselectAtom( *it );
			}

			p_atoms[ 0 ]->getMoleculePtr()->refreshSelection( &_items[ p_atoms[ 0 ]->getMoleculePtr()->getId() ] );
			_notifyDataChanged();
		}

		void Selection::unselectRepresentation( Representation::InstantiatedRepresentation & p_representation )
		{
			_unselectRepresentation( p_representation );
			_notifyDataChanged();
		}
		void Selection::unselectRepresentations(
			const std::vector<Representation::InstantiatedRepresentation *> & p_representations )
		{
			if ( p_representations.size() == 0 )
				return;

			for ( const auto it : p_representations )
				_unselectRepresentation( *it );

			_notifyDataChanged();
		}
		void Selection::unselectRepresentationsWithCheck(
			const std::vector<Representation::InstantiatedRepresentation *> & p_representations )
		{
			for ( const auto it : p_representations )
			{
				if ( isRepresentationSelected( *it ) )
					_unselectRepresentation( *it );
			}

			_notifyDataChanged();
		}

		bool Selection::isMoleculeSelected( const Molecule & p_molecule ) const
		{
			const ID & id = p_molecule.getId();
			return _items.find( id ) != _items.end();
		}
		bool Selection::isChainSelected( const Chain & p_chain ) const
		{
			const ID & moleculeId = p_chain.getMoleculePtr()->getId();

			if ( _items.find( moleculeId ) == _items.end() )
				return false;

			const MapChainIds & chainMap = _items.at( moleculeId );
			const uint &		index	 = p_chain.getIndex();

			return chainMap.find( index ) != chainMap.end();
		}
		bool Selection::isResidueSelected( const Residue & p_residue ) const
		{
			const ID & moleculeId = p_residue.getMoleculePtr()->getId();

			if ( _items.find( moleculeId ) == _items.end() )
				return false;

			const MapChainIds & chainMap   = _items.at( moleculeId );
			const uint &		chainIndex = p_residue.getChainPtr()->getIndex();

			if ( chainMap.find( chainIndex ) == chainMap.end() )
				return false;

			const MapResidueIds & residueMap = _items.at( moleculeId ).at( chainIndex );
			const uint &		  index		 = p_residue.getIndex();

			return residueMap.find( index ) != residueMap.end();
		}
		bool Selection::isAtomSelected( const Atom & p_atom ) const
		{
			const ID & moleculeId = p_atom.getMoleculePtr()->getId();

			if ( _items.find( moleculeId ) == _items.end() )
				return false;

			const MapChainIds & chainMap   = _items.at( moleculeId );
			const uint &		chainIndex = p_atom.getChainPtr()->getIndex();

			if ( chainMap.find( chainIndex ) == chainMap.end() )
				return false;

			const MapResidueIds & residueMap   = _items.at( moleculeId ).at( chainIndex );
			const uint &		  residueIndex = p_atom.getResiduePtr()->getIndex();
			if ( residueMap.find( residueIndex ) == residueMap.end() )
				return false;

			const VecAtomIds & atomVector = _items.at( moleculeId ).at( chainIndex ).at( residueIndex );
			const uint &	   index	  = p_atom.getIndex();

			return std::find( atomVector.begin(), atomVector.end(), index ) != atomVector.end();
		}
		bool Selection::isRepresentationSelected( Representation::InstantiatedRepresentation & p_representation ) const
		{
			return _representations.find( &p_representation ) != _representations.end();
		}

		uint Selection::getMoleculeSelectedCount() const { return (uint)_items.size(); }
		uint Selection::getChainSelectedCount() const
		{
			uint res = 0;
			for ( const std::pair<ID, MapChainIds> mapMolecules : _items )
				res += (uint)mapMolecules.second.size();
			return res;
		}
		uint Selection::getResidueSelectedCount() const
		{
			uint res = 0;
			for ( const std::pair<ID, MapChainIds> mapMolecules : _items )
				for ( const std::pair<ID, MapResidueIds> mapChains : mapMolecules.second )
					res += (uint)mapChains.second.size();
			return res;
		}
		uint Selection::getAtomSelectedCount() const
		{
			uint res = 0;
			for ( const std::pair<ID, MapChainIds> mapMolecules : _items )
				for ( const std::pair<ID, MapResidueIds> mapChains : mapMolecules.second )
					for ( const std::pair<ID, VecAtomIds> mapResidus : mapChains.second )
						res += (uint)mapResidus.second.size();
			return res;
		}
		uint Selection::getRepresentationSelectedCount() const { return (uint)_representations.size(); }

		void Selection::_selectMolecule( const Molecule & p_molecule )
		{
			_addMolecule( p_molecule );
			_addMoleculeContent( p_molecule );

			_aabb.extend( p_molecule.getAABB() );
		}

		void Selection::_selectChain( const Chain & p_chain )
		{
			_addMolecule( *p_chain.getMoleculePtr() );
			_addChain( p_chain );
			_addChainContent( p_chain );

			_aabb.extend( p_chain.getAABB() );
		}

		void Selection::_selectResidue( const Residue & p_residue )
		{
			_addMolecule( *p_residue.getMoleculePtr() );
			_addChain( *p_residue.getChainPtr() );
			_addResidue( p_residue );
			_addResidueContent( p_residue );

			_aabb.extend( p_residue.getAABB() );
		}

		void Selection::_selectAtom( const Atom & p_atom )
		{
			_addMolecule( *p_atom.getMoleculePtr() );
			_addChain( *p_atom.getChainPtr() );
			_addResidue( *p_atom.getResiduePtr() );
			_addAtom( p_atom );

			_aabb.extend( p_atom.getAABB() );
		}

		void Selection::_unselectMolecule( const Molecule & p_molecule )
		{
			_removeMolecule( p_molecule );
			_recomputeAABB();
		}

		void Selection::_unselectChain( const Chain & p_chain )
		{
			_removeChain( p_chain );
			_recomputeAABB();
		}

		void Selection::_unselectResidue( const Residue & p_residue )
		{
			_removeResidue( p_residue );
			_recomputeAABB();
		}

		void Selection::_unselectAtom( const Atom & p_atom )
		{
			_removeAtom( p_atom );
			_recomputeAABB();
		}

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
				const Chain * const chain = p_molecule.getChain( i );

				if ( chain == nullptr )
					continue;

				_addChain( *chain );
				_addChainContent( *chain );
			}
		}

		void Selection::_addChainContent( const Chain & p_chain )
		{
			for ( uint i = 0; i < p_chain.getResidueCount(); ++i )
			{
				const Residue * const residue
					= p_chain.getMoleculePtr()->getResidue( p_chain.getIndexFirstResidue() + i );

				if ( residue == nullptr )
					continue;

				_addResidue( *residue );
				_addResidueContent( *residue );
			}
		}

		void Selection::_addResidueContent( const Residue & p_residue )
		{
			for ( uint i = 0; i < p_residue.getAtomCount(); ++i )
			{
				const Atom * const atom = p_residue.getMoleculePtr()->getAtom( p_residue.getIndexFirstAtom() + i );

				if ( atom == nullptr )
					continue;

				_addAtom( *atom );
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

		void Selection::_selectRepresentation( Representation::InstantiatedRepresentation & p_representation )
		{
			_representations.emplace( &p_representation );
		}
		void Selection::_unselectRepresentation( Representation::InstantiatedRepresentation & p_representation )
		{
			_representations.erase( &p_representation );
		}

		void Selection::clear()
		{
			_clearWithoutNotify();
			this->_notifyDataChanged();
		}
		void Selection::_clearWithoutNotify()
		{
			for ( const std::pair<ID, MapChainIds> item : _items )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( item.first );
				molecule.refreshSelection( nullptr );
			}

			_items.clear();
			_representations.clear();
			_aabb = Math::AABB();
		}

		void Selection::receiveEvent( const Event::VTXEvent & p_event )
		{
			if ( p_event.name == Event::MOLECULE_REMOVED )
			{
				const Event::VTXEventPtr<Model::Molecule> & castedEvent
					= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
				unselectMolecule( *castedEvent.ptr );
			}
			else if ( p_event.name == Event::MOLECULE_STRUCTURE_CHANGE )
			{
				const Event::VTXEventPtr<Model::Molecule> & castedEvent
					= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
				unselectMolecule( *castedEvent.ptr );
			}
			else if ( p_event.name == Event::REPRESENTATION_REMOVED )
			{
				const Event::VTXEventPtr<Model::Representation::InstantiatedRepresentation> & castedEvent
					= dynamic_cast<const Event::VTXEventPtr<Model::Representation::InstantiatedRepresentation> &>(
						p_event );
				unselectRepresentation( *castedEvent.ptr );
			}
		}

		void Selection::_notifyDataChanged()
		{
			BaseModel::_notifyDataChanged();
			VTX_EVENT( new Event::VTXEventPtr( Event ::SELECTION_CHANGE, this ) );
		}

		void Selection::_recomputeAABB()
		{
			_aabb = Math::AABB();

			for ( const std::pair<ID, MapChainIds> & mapMol : _items )
			{
				const Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( mapMol.first );
				if ( molecule.getChainCount() == mapMol.second.size() )
				{
					_aabb.extend( molecule.getAABB() );
					continue;
				}

				for ( const std::pair<ID, MapResidueIds> & mapChain : mapMol.second )
				{
					const Model::Chain & chain = *molecule.getChain( mapChain.first );
					if ( chain.getResidueCount() == mapChain.second.size() )
					{
						_aabb.extend( chain.getAABB() );
						continue;
					}

					for ( const std::pair<ID, VecAtomIds> & mapResidue : mapChain.second )
					{
						const Model::Residue & residue = *molecule.getResidue( mapResidue.first );
						if ( residue.getAtomCount() == mapResidue.second.size() )
						{
							_aabb.extend( residue.getAABB() );
							continue;
						}

						for ( const uint & vectorId : mapResidue.second )
						{
							const Model::Atom & atom = *molecule.getAtom( vectorId );
							_aabb.extend( atom.getAABB() );
						}
					}
				}
			}
		}
	} // namespace Model
} // namespace VTX
