#include "app/old_app/representation/representation_manager.hpp"
#include "app/core/mvc/mvc_manager.hpp"
#include "app/event.hpp"
#include "app/model/chain.hpp"
#include "app/model/molecule.hpp"
#include "app/model/representation/instantiated_representation.hpp"
#include "app/model/representation/representation.hpp"
#include "app/model/representation/representation_library.hpp"
#include "app/model/residue.hpp"
#include "app/model/selection.hpp"
#include "app/old_app/generic/base_representable.hpp"
#include "app/old_app/setting.hpp"
#include "app/view/callback_view.hpp"

namespace VTX::Representation
{
	RepresentationManager::RepresentationManager() {}

	RepresentationManager::InstantiatedRepresentation * RepresentationManager::instantiateDefaultRepresentation(
		Generic::BaseRepresentable & p_target,
		const bool					 p_recompute,
		const bool					 p_notify )
	{
		const Representation * const defaultRepresentation = RepresentationLibrary::get().getDefaultRepresentation();
		return instantiateRepresentation( defaultRepresentation, p_target, p_recompute, p_notify );
	}

	RepresentationManager::InstantiatedRepresentation * RepresentationManager::instantiateRepresentation(
		const Representation * const p_representation,
		Generic::BaseRepresentable & p_target,
		const bool					 p_recompute,
		const bool					 p_notify )
	{
		if ( p_target.getMolecule() == &p_target )
		{
			p_target.getMolecule()->clearDefaultRepresentations();
		}

		InstantiatedRepresentation * const instantiatedRepresentation
			= VTX::Core::MVC::MvcManager::get().instantiateModel<InstantiatedRepresentation>( p_representation );

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

	void RepresentationManager::instantiateRepresentations( const Representation * const p_representation,
															const Model::Selection &	 p_selection )
	{
		for ( const Model::Selection::PairMoleculeIds & molData : p_selection.getMoleculesMap() )
		{
			Model::Molecule & molecule = VTX::Core::MVC::MvcManager::get().getModel<Model::Molecule>( molData.first );
			if ( molData.second.getFullySelectedChildCount() == molecule.getRealChainCount() )
			{
				instantiateRepresentation( p_representation, molecule, false, true );
			}
			else
			{
				for ( const Model::Selection::PairChainIds & chainData : molData.second )
				{
					Model::Chain & chain = *molecule.getChain( chainData.first );
					if ( chainData.second.getFullySelectedChildCount() == chain.getRealResidueCount() )
					{
						instantiateRepresentation( p_representation, chain, false, true );
					}
					else
					{
						for ( const Model::Selection::PairResidueIds & residueData : chainData.second )
						{
							Model::Residue & residue = *molecule.getResidue( residueData.first );
							instantiateRepresentation( p_representation, residue, false, true );
						}
					}
				}
			}

			molecule.computeAllRepresentationData();
		}
	}
	RepresentationManager::InstantiatedRepresentation * RepresentationManager::instantiateCopy(
		const InstantiatedRepresentation * const p_source,
		Generic::BaseRepresentable &			 p_target,
		const bool								 p_recompute,
		const bool								 p_notify )
	{
		InstantiatedRepresentation * const copy
			= VTX::Core::MVC::MvcManager::get().instantiateModel<InstantiatedRepresentation>(
				p_source->getLinkedRepresentation() );

		copy->copy( *p_source );

		assignRepresentation( copy, p_target, p_recompute, p_notify );

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
		while ( RepresentationLibrary::get().getRepresentationCount() > 0 )
		{
			const Representation * const representation = RepresentationLibrary::get().getRepresentation( 0 );
			if ( _mapRepresentationInstances.find( representation ) != _mapRepresentationInstances.end() )
			{
				_mapRepresentationInstances.erase( representation );
				_deleteViewOnRepresentation( representation );
			}

			RepresentationLibrary::get().deleteRepresentation( 0, p_notify );
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

		const int representationIndex = RepresentationLibrary::get().getRepresentationIndex( p_representation );

		if ( VTX_SETTING().getDefaultRepresentationIndex() == representationIndex )
			RepresentationLibrary::get().setDefaultRepresentation( representationIndex - 1, false );

		RepresentationLibrary::get().deleteRepresentation( representationIndex );
	}

	void RepresentationManager::_instantiateViewOnRepresentation( const Representation * const p_representation )
	{
		View::CallbackView<const Representation, RepresentationManager> * const view
			= VTX::Core::MVC::MvcManager::get()
				  .instantiateView<View::CallbackView<const Representation, RepresentationManager>>(
					  p_representation, ID::View::INSTANTIATED_REPRESENTATION_ON_REPRESENTATION );

		view->setCallback( this, &RepresentationManager::_onRepresentationChange );
	}
	void RepresentationManager::_deleteViewOnRepresentation( const Representation * const p_representation ) const
	{
		VTX::Core::MVC::MvcManager::get().deleteView( p_representation,
													  ID::View::INSTANTIATED_REPRESENTATION_ON_REPRESENTATION );
	}

	void RepresentationManager::_onRepresentationChange( const Representation * const			  p_representation,
														 const App::Core::Event::VTXEvent * const p_event )
	{
		if ( p_event->name == App::Event::Model::REPRESENTATION_TYPE_CHANGE )
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
		const int  representationIndex = RepresentationLibrary::get().getRepresentationIndex( p_defaultRepresentation );
		const bool isLastRep = representationIndex >= ( RepresentationLibrary::get().getRepresentationCount() - 1 );
		const int  newRepresentationIndex = isLastRep ? representationIndex - 1 : representationIndex + 1;

		const Representation * const newRepresentation
			= RepresentationLibrary::get().getRepresentation( newRepresentationIndex );

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
		for ( const Representation * representation : RepresentationLibrary::get().getRepresentations() )
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
			= VTX::Core::MVC::MvcManager::get().instantiateModel<InstantiatedRepresentation>(
				p_source.getLinkedRepresentation() );

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

		for ( Representation * const representation : RepresentationLibrary::get().getRepresentations() )
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
		const Model::Representation::RepresentationLibrary & representations
			= Model::Representation::RepresentationLibrary::get();

		const RepresentationManager::Representation * const representation
			= representations.getRepresentationByName( Setting::REPRESENTATION_DEFAULT_NAME );

		if ( representation == nullptr )
			return 0;
		else
			return representations.getRepresentationIndex( representation );
	}

	void RepresentationManager::resetRepresentations()
	{
		storeRepresentations();
		clearAllRepresentations( false );
		RepresentationLibrary::get().resetToDefault( false );
		restoreRepresentations();
		RepresentationLibrary::get().forceNotifyDataChanged();
	}

} // namespace VTX::Representation
