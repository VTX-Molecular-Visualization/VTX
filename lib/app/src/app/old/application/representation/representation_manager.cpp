#include "app/old/application/representation/representation_manager.hpp"
#include "app/old/application/representation/base_representable.hpp"
#include "app/old/application/representation/instantiated_representation.hpp"
#include "app/old/application/representation/representation_library.hpp"
#include "app/old/application/representation/representation_preset.hpp"
#include "app/old/application/selection/selection.hpp"
#include "app/old/application/setting.hpp"
#include "app/old/component/chemistry/chain.hpp"
#include "app/old/component/chemistry/molecule.hpp"
#include "app/old/component/chemistry/residue.hpp"
#include "app/old/core/view/callback_view.hpp"
#include "app/old/event.hpp"
#include "app/old/mvc.hpp"

namespace VTX::App::Old::Application::Representation
{
	RepresentationManager::RepresentationManager() {}

	InstantiatedRepresentation * RepresentationManager::instantiateDefaultRepresentation( BaseRepresentable & p_target,
																						  const bool p_recompute,
																						  const bool p_notify )
	{
		const RepresentationPreset * const defaultRepresentation
			= RepresentationLibrary::get().getDefaultRepresentation();
		return instantiateRepresentation( defaultRepresentation, p_target, p_recompute, p_notify );
	}

	InstantiatedRepresentation * RepresentationManager::instantiateRepresentation(
		const RepresentationPreset * const p_representation,
		BaseRepresentable &				   p_target,
		const bool						   p_recompute,
		const bool						   p_notify )
	{
		if ( p_target.getMolecule() == &p_target )
		{
			p_target.getMolecule()->clearDefaultRepresentations();
		}

		InstantiatedRepresentation * const instantiatedRepresentation
			= VTX::MVC_MANAGER().instantiateModel<InstantiatedRepresentation>( p_representation );

		assignRepresentation( instantiatedRepresentation, p_target, p_recompute, p_notify );

		return instantiatedRepresentation;
	}

	void RepresentationManager::assignRepresentation( InstantiatedRepresentation * const p_instantiatedRepresentation,
													  BaseRepresentable &				 p_target,
													  const bool						 p_recompute,
													  const bool						 p_notify )
	{
		removeInstantiatedRepresentation( p_target, false, false );

		p_target.applyRepresentation( p_instantiatedRepresentation, p_recompute, p_notify );

		const RepresentationPreset * const representation = p_instantiatedRepresentation->getLinkedRepresentation();

		if ( _mapRepresentationInstances.find( representation ) == _mapRepresentationInstances.end() )
			_instantiateViewOnRepresentation( representation );

		_mapRepresentationInstances[ representation ].emplace( p_instantiatedRepresentation );
	};

	void RepresentationManager::applyRepresentation( BaseRepresentable &				p_representable,
													 const InstantiatedRepresentation & p_source,
													 const MEMBER_FLAG &				p_flag,
													 const bool							p_recompute )
	{
		if ( p_representable.hasCustomRepresentation() )
		{
			p_representable.getCustomRepresentation()->applyData( p_source, p_flag, p_recompute );
		}
		else
		{
			InstantiatedRepresentation * instantiatedRepresentation
				= instantiateCopy( &p_source, p_representable, p_recompute, true );
		}
	};

	void RepresentationManager::instantiateRepresentations(
		const RepresentationPreset * const					p_representation,
		const App::Old::Application::Selection::SelectionModel & p_selection )
	{
		for ( const App::Old::Application::Selection::SelectionModel::PairMoleculeIds & molData :
			  p_selection.getMoleculesMap() )
		{
			App::Old::Component::Chemistry::Molecule & molecule
				= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( molData.first );
			if ( molData.second.getFullySelectedChildCount() == molecule.getRealChainCount() )
			{
				instantiateRepresentation( p_representation, molecule, false, true );
			}
			else
			{
				for ( const App::Old::Application::Selection::SelectionModel::PairChainIds & chainData : molData.second )
				{
					App::Old::Component::Chemistry::Chain & chain = *molecule.getChain( chainData.first );
					if ( chainData.second.getFullySelectedChildCount() == chain.getRealResidueCount() )
					{
						instantiateRepresentation( p_representation, chain, false, true );
					}
					else
					{
						for ( const App::Old::Application::Selection::SelectionModel::PairResidueIds & residueData :
							  chainData.second )
						{
							App::Old::Component::Chemistry::Residue & residue = *molecule.getResidue( residueData.first );
							instantiateRepresentation( p_representation, residue, false, true );
						}
					}
				}
			}

			molecule.computeAllRepresentationData();
		}
	}
	InstantiatedRepresentation * RepresentationManager::instantiateCopy(
		const InstantiatedRepresentation * const p_source,
		BaseRepresentable &						 p_target,
		const bool								 p_recompute,
		const bool								 p_notify )
	{
		InstantiatedRepresentation * const copy
			= VTX::MVC_MANAGER().instantiateModel<InstantiatedRepresentation>( p_source->getLinkedRepresentation() );

		copy->copy( *p_source );

		assignRepresentation( copy, p_target, p_recompute, p_notify );

		copy->setTarget( &p_target );

		return copy;
	}

	/*void RepresentationManager::removeInstantiatedRepresentation( BaseRepresentable
	& p_representable, const bool				   p_recompute )
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
			const RepresentationPreset * const representation = RepresentationLibrary::get().getRepresentation( 0 );
			if ( _mapRepresentationInstances.find( representation ) != _mapRepresentationInstances.end() )
			{
				_mapRepresentationInstances.erase( representation );
				_deleteViewOnRepresentation( representation );
			}

			RepresentationLibrary::get().deleteRepresentation( 0, p_notify );
		}
	}

	void RepresentationManager::deleteRepresentation( const RepresentationPreset * const p_representation )
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

	void RepresentationManager::_instantiateViewOnRepresentation( const RepresentationPreset * const p_representation )
	{
		App::Old::Core::View::CallbackView<const RepresentationPreset, RepresentationManager> * const view
			= VTX::MVC_MANAGER()
				  .instantiateView<App::Old::Core::View::CallbackView<const RepresentationPreset, RepresentationManager>>(
					  p_representation, ID::View::INSTANTIATED_REPRESENTATION_ON_REPRESENTATION );

		view->setCallback( this, &RepresentationManager::_onRepresentationChange );
	}
	void RepresentationManager::_deleteViewOnRepresentation( const RepresentationPreset * const p_representation ) const
	{
		VTX::MVC_MANAGER().deleteView( p_representation, ID::View::INSTANTIATED_REPRESENTATION_ON_REPRESENTATION );
	}

	void RepresentationManager::_onRepresentationChange( const RepresentationPreset * const		  p_representation,
														 const App::Old::Core::Event::VTXEvent * const p_event )
	{
		if ( p_event->name == App::Old::Event::Model::REPRESENTATION_TYPE_CHANGE )
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
		const RepresentationPreset * const p_defaultRepresentation )
	{
		const int  representationIndex = RepresentationLibrary::get().getRepresentationIndex( p_defaultRepresentation );
		const bool isLastRep = representationIndex >= ( RepresentationLibrary::get().getRepresentationCount() - 1 );
		const int  newRepresentationIndex = isLastRep ? representationIndex - 1 : representationIndex + 1;

		const RepresentationPreset * const newRepresentation
			= RepresentationLibrary::get().getRepresentation( newRepresentationIndex );

		std::unordered_set<App::Old::Component::Chemistry::Molecule *> molecules
			= std::unordered_set<App::Old::Component::Chemistry::Molecule *>();

		while ( _mapRepresentationInstances[ p_defaultRepresentation ].size() > 0 )
		{
			InstantiatedRepresentation * const instantiatedRepresentation
				= *_mapRepresentationInstances[ p_defaultRepresentation ].begin();

			BaseRepresentable & target = *instantiatedRepresentation->getTarget();
			molecules.emplace( target.getMolecule() );

			removeInstantiatedRepresentation( target, false, false );
			instantiateRepresentation( newRepresentation, target, false, true );
		}

		for ( App::Old::Component::Chemistry::Molecule * const molecule : molecules )
		{
			molecule->computeAllRepresentationData();
		}
	}

	InstantiatedRepresentation * const RepresentationManager::getRepresentationByName(
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

	void RepresentationManager::setQuickAccessToPreset( RepresentationPreset * const p_representation,
														const bool					 p_quickAccess )
	{
		if ( p_quickAccess )
		{
			const int quickAccessCount = _getRepresentationWithQuickAccessCount();

			if ( quickAccessCount >= VTX::App::Old::Application::Setting::MAX_QUICK_ACCESS_COUNT
				 && _lastRepresentationQuickAccessed != nullptr )
				_lastRepresentationQuickAccessed->setQuickAccess( false );
		}

		p_representation->setQuickAccess( p_quickAccess );

		if ( p_quickAccess )
			_lastRepresentationQuickAccessed = p_representation;
	}
	int RepresentationManager::_getRepresentationWithQuickAccessCount() const
	{
		int res = 0;
		for ( const RepresentationPreset * representation : RepresentationLibrary::get().getRepresentations() )
		{
			if ( representation->hasQuickAccess() )
				res++;
		}
		return res;
	}

	InstantiatedRepresentation * RepresentationManager::instantiateDummy( const InstantiatedRepresentation & p_source )
	{
		InstantiatedRepresentation * const dummy
			= VTX::MVC_MANAGER().instantiateModel<InstantiatedRepresentation>( p_source.getLinkedRepresentation() );

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

		for ( RepresentationPreset * const representation : RepresentationLibrary::get().getRepresentations() )
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
		const RepresentationLibrary & representations = RepresentationLibrary::get();

		const RepresentationPreset * const representation
			= representations.getRepresentationByName( VTX::App::Old::Application::Setting::REPRESENTATION_DEFAULT_NAME );

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

} // namespace VTX::App::Old::Application::Representation
