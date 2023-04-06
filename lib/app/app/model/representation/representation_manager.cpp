#include "representation_manager.hpp"
#include "default_values.hpp"
#include "generic/base_representable.hpp"
#include "manager/event/event_manager.hpp"
#include "manager/mvc/mvc_manager.hpp"
#include "model/chemistry/chain.hpp"
#include "model/chemistry/molecule.hpp"
#include "model/chemistry/residue.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
// #include "model/selection.hpp"
// #include "view/callback_view.hpp"

namespace VTX::Representation
{
	RepresentationManager::RepresentationManager() {}

	RepresentationManager::InstantiatedRepresentation * RepresentationManager::instantiateDefaultRepresentation(
		Generic::BaseRepresentable & p_target,
		const bool					 p_recompute,
		const bool					 p_notify )
	{
		const Representation * const defaultRepresentation = _representationLibrary->getDefaultRepresentation();
		return instantiateRepresentation( defaultRepresentation, p_target, p_recompute, p_notify );
	}

	RepresentationManager::InstantiatedRepresentation * RepresentationManager::instantiateRepresentation(
		const Representation * const p_representation,
		Generic::BaseRepresentable & p_target,
		const bool					 p_recompute,
		const bool					 p_notify )
	{
		InstantiatedRepresentation * const instantiatedRepresentation
			= MVC::MvcManager::get().instantiateModel<InstantiatedRepresentation>( p_representation );

		assignRepresentation( instantiatedRepresentation, p_target, p_recompute, p_notify );

		return instantiatedRepresentation;
	}

	void RepresentationManager::assignRepresentation( InstantiatedRepresentation * const p_instantiatedRepresentation,
													  Generic::BaseRepresentable &		 p_target,
													  const bool						 p_recompute,
													  const bool						 p_notify )
	{
		removeInstantiatedRepresentation( p_target, false, false );

		p_target.applyRepresentation( p_instantiatedRepresentation, p_recompute, p_notify );

		const Representation * const representation = p_instantiatedRepresentation->getLinkedRepresentation();

		if ( _mapRepresentationInstances.find( representation ) == _mapRepresentationInstances.end() )
			_instantiateViewOnRepresentation( representation );

		_mapRepresentationInstances[ representation ].emplace( p_instantiatedRepresentation );
	};

	void RepresentationManager::applyRepresentation( Generic::BaseRepresentable &				p_representable,
													 const InstantiatedRepresentation &			p_source,
													 const Model::Representation::MEMBER_FLAG & p_flag,
													 const bool									p_recompute )
	{
		if ( p_representable.hasCustomRepresentation() )
		{
			p_representable.getCustomRepresentation()->applyData( p_source, p_flag, p_recompute );
		}
		else
		{
			Model::Representation::InstantiatedRepresentation * instantiatedRepresentation
				= instantiateCopy( &p_source, p_representable, p_recompute, true );
		}
	};

	// TODO VTX_LOGIC Manage representation instantiation on Selection out of this class ( Directly in the
	// Action::Selection ?) void RepresentationManager::instantiateRepresentations( const Representation * const
	// p_representation, 														const Model::Selection &	 p_selection
	// )
	//{
	//	for ( const Model::Selection::PairMoleculeIds & molData : p_selection.getMoleculesMap() )
	//	{
	//		Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molData.first );
	//		if ( molData.second.getFullySelectedChildCount() == molecule.getRealChainCount() )
	//		{
	//			instantiateRepresentation( p_representation, molecule, false, true );
	//		}
	//		else
	//		{
	//			for ( const Model::Selection::PairChainIds & chainData : molData.second )
	//			{
	//				Model::Chain & chain = *molecule.getChain( chainData.first );
	//				if ( chainData.second.getFullySelectedChildCount() == chain.getRealResidueCount() )
	//				{
	//					instantiateRepresentation( p_representation, chain, false, true );
	//				}
	//				else
	//				{
	//					for ( const Model::Selection::PairResidueIds & residueData : chainData.second )
	//					{
	//						Model::Residue & residue = *molecule.getResidue( residueData.first );
	//						instantiateRepresentation( p_representation, residue, false, true );
	//					}
	//				}
	//			}
	//		}

	//		molecule.computeAllRepresentationData();
	//	}
	//}

	RepresentationManager::InstantiatedRepresentation * RepresentationManager::instantiateCopy(
		const InstantiatedRepresentation * const p_source,
		Generic::BaseRepresentable &			 p_target,
		const bool								 p_recompute,
		const bool								 p_notify )
	{
		InstantiatedRepresentation * const copy
			= instantiateRepresentation( p_source->getLinkedRepresentation(), p_target, p_recompute, p_notify );
		copy->copy( *p_source );
		copy->setTarget( &p_target );

		return copy;
	}

	/*void RepresentationManager::removeInstantiatedRepresentation( Generic::BaseRepresentable & p_representable,
																  const bool				   p_recompute )
	{
		if ( !p_representable.hasCustomRepresentation() )
			return;

		InstantiatedRepresentation * const instantiatedRepresentation = p_representable.getCustomRepresentation();
		const Representation * const	   representation = instantiatedRepresentation->getLinkedRepresentation();

		_mapRepresentationInstances[ representation ].erase( instantiatedRepresentation );

		p_representable.removeRepresentation( false );

		if ( p_recompute )
			p_representable.getMolecule()->computeAllRepresentationData();
	}*/

	void RepresentationManager::clearAllRepresentations( const bool p_notify )
	{
		while ( _representationLibrary->getRepresentationCount() > 0 )
		{
			const Representation * const representation = _representationLibrary->getRepresentation( 0 );
			if ( _mapRepresentationInstances.find( representation ) != _mapRepresentationInstances.end() )
			{
				_mapRepresentationInstances.erase( representation );
				_deleteViewOnRepresentation( representation );
			}

			_representationLibrary->deleteRepresentation( 0, p_notify );
		}
	}

	void RepresentationManager::deleteRepresentation( const Representation * const p_representation )
	{
		if ( _mapRepresentationInstances.find( p_representation ) != _mapRepresentationInstances.end() )
		{
			_applyNextBaseRepresentationOnInstances( p_representation );

			_mapRepresentationInstances.erase( p_representation );
			_deleteViewOnRepresentation( p_representation );
		}

		const int representationIndex = _representationLibrary->getRepresentationIndex( p_representation );

		// TODO VTX_LOGIC : Manage default representation in RepresentationLibrary or RepresentationManager, but don't
		// access to setting from here
		// if ( VTX_SETTING().getDefaultRepresentationIndex() == representationIndex )
		//	_representationLibrary->setDefaultRepresentation( representationIndex - 1, false );

		_representationLibrary->deleteRepresentation( representationIndex );
	}

	void RepresentationManager::_instantiateViewOnRepresentation( const Representation * const p_representation )
	{
		// TODO VTX_LOGIC manage that with simple event instead of fake view
		// View::CallbackView<const Representation, RepresentationManager>* const view
		//	= MVC::MvcManager::get().instantiateView<View::CallbackView<const Representation, RepresentationManager>>(
		//		p_representation, ID::View::INSTANTIATED_REPRESENTATION_ON_REPRESENTATION );

		// view->setCallback( this, &RepresentationManager::_onRepresentationChange );
	}
	void RepresentationManager::_deleteViewOnRepresentation( const Representation * const p_representation ) const
	{
		MVC::MvcManager::get().deleteView( p_representation, ID::View::INSTANTIATED_REPRESENTATION_ON_REPRESENTATION );
	}

	void RepresentationManager::_onRepresentationChange( const Representation * const  p_representation,
														 const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::REPRESENTATION_TYPE_CHANGE )
		{
			for ( InstantiatedRepresentation * const instantiatedRepresentation :
				  _mapRepresentationInstances[ p_representation ] )
			{
				instantiatedRepresentation->refreshSourceValues();
			}
		}

		for ( InstantiatedRepresentation * const instantiatedRepresentation :
			  _mapRepresentationInstances[ p_representation ] )
		{
			instantiatedRepresentation->onLinkedRepresentationChange( p_event );
		}
	}

	void RepresentationManager::_applyNextBaseRepresentationOnInstances(
		const Representation * const p_defaultRepresentation )
	{
		const int  representationIndex = _representationLibrary->getRepresentationIndex( p_defaultRepresentation );
		const bool isLastRep = representationIndex >= ( _representationLibrary->getRepresentationCount() - 1 );
		const int  newRepresentationIndex = isLastRep ? representationIndex - 1 : representationIndex + 1;

		const Representation * const newRepresentation
			= _representationLibrary->getRepresentation( newRepresentationIndex );

		std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

		while ( _mapRepresentationInstances[ p_defaultRepresentation ].size() > 0 )
		{
			InstantiatedRepresentation * const instantiatedRepresentation
				= *_mapRepresentationInstances[ p_defaultRepresentation ].begin();

			Generic::BaseRepresentable & target = *instantiatedRepresentation->getTarget();
			molecules.emplace( target.getMolecule() );

			removeInstantiatedRepresentation( target, false, false );
			instantiateRepresentation( newRepresentation, target, false, true );
		}

		for ( Model::Molecule * const molecule : molecules )
		{
			molecule->computeAllRepresentationData();
		}
	}

	RepresentationManager::InstantiatedRepresentation * const RepresentationManager::getRepresentationByName(
		const std::string & p_representationName ) const
	{
		InstantiatedRepresentation * res = nullptr;

		for ( const PairRepresentationRepresentables & item : _mapRepresentablesLinkedToRepresentation )
		{
			if ( item.first->getName().compare( p_representationName ) == 0 )
			{
				res = item.first;
				break;
			}
		}

		return res;
	}

	void RepresentationManager::setQuickAccessToPreset( Representation * const p_representation,
														const bool			   p_quickAccess )
	{
		if ( p_quickAccess )
		{
			const int quickAccessCount = _getRepresentationWithQuickAccessCount();

			if ( quickAccessCount >= Setting::MAX_QUICK_ACCESS_COUNT && _lastRepresentationQuickAccessed != nullptr )
				_lastRepresentationQuickAccessed->setQuickAccess( false );
		}

		p_representation->setQuickAccess( p_quickAccess );

		if ( p_quickAccess )
			_lastRepresentationQuickAccessed = p_representation;
	}
	int RepresentationManager::_getRepresentationWithQuickAccessCount() const
	{
		int res = 0;
		for ( const Representation * representation : _representationLibrary->getRepresentations() )
		{
			if ( representation->hasQuickAccess() )
				res++;
		}
		return res;
	}

	RepresentationManager::InstantiatedRepresentation * RepresentationManager::instantiateDummy(
		const InstantiatedRepresentation & p_source )
	{
		InstantiatedRepresentation * const dummy
			= MVC::MvcManager::get().instantiateModel<InstantiatedRepresentation>( p_source.getLinkedRepresentation() );

		dummy->copy( p_source );
		return dummy;
	}

	void RepresentationManager::storeRepresentations()
	{
		_storedRepresentations.clear();

		for ( const PairRepresentationInstances & instantiatedRepresentationPerRepresentation :
			  _mapRepresentationInstances )
		{
			_storedRepresentations.emplace( instantiatedRepresentationPerRepresentation.first->getName(),
											instantiatedRepresentationPerRepresentation.second );
		}
	}

	void RepresentationManager::restoreRepresentations()
	{
		_mapRepresentationInstances.clear();

		for ( Representation * const representation : _representationLibrary->getRepresentations() )
		{
			MapStoredRepresentation::iterator it = _storedRepresentations.find( representation->getName() );
			if ( it != _storedRepresentations.end() )
			{
				_instantiateViewOnRepresentation( representation );
				_mapRepresentationInstances.emplace( representation, it->second );

				for ( InstantiatedRepresentation * const instantiatedRepresentation : it->second )
				{
					instantiatedRepresentation->setLinkedRepresentation( representation, false );
					instantiatedRepresentation->forceNotifyDataChanged();
				}

				it = _storedRepresentations.erase( it );
			}
		}

		for ( const PairStoredRepresentation & storedRepresentation : _storedRepresentations )
		{
			for ( InstantiatedRepresentation * const instantiatedRepresentation : storedRepresentation.second )
			{
				if ( instantiatedRepresentation->getConstTarget()->hasParent() )
					removeInstantiatedRepresentation( *instantiatedRepresentation->getTarget() );
				else
					instantiateDefaultRepresentation( *instantiatedRepresentation->getTarget() );
			}
		}

		_storedRepresentations.clear();
	}

	int RepresentationManager::getDefaultRepresentationIndex() const
	{
		const RepresentationManager::Representation * const representation
			= _representationLibrary->getRepresentationByName( Setting::REPRESENTATION_DEFAULT_NAME );

		if ( representation == nullptr )
			return 0;
		else
			return _representationLibrary->getRepresentationIndex( representation );
	}

	void RepresentationManager::resetRepresentations()
	{
		storeRepresentations();
		clearAllRepresentations( false );
		_representationLibrary->resetToDefault( false );
		restoreRepresentations();
		_representationLibrary->forceNotifyDataChanged();
	}

} // namespace VTX::Representation
