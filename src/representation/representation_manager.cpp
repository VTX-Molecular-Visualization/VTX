#include "representation_manager.hpp"
#include "event/event_manager.hpp"
#include "generic/base_representable.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "setting.hpp"
#include "view/callback_view.hpp"

namespace VTX::Representation
{
	RepresentationManager::RepresentationManager()
	{
		setDefaultRepresentationIndex( VTX::Setting::REPRESENTATION_DEFAULT_INDEX );
	}

	void RepresentationManager::setDefaultRepresentationIndex( const int p_defaultRepresentationIndex )
	{
		_defaultBaseRepresentation
			= Model::Representation::RepresentationLibrary::get().getRepresentation( p_defaultRepresentationIndex );
		_instantiateViewOnRepresentation( _defaultBaseRepresentation );
	}

	RepresentationManager::InstantiatedRepresentation * RepresentationManager::instantiateDefaultRepresentation(
		Generic::BaseRepresentable & p_target )
	{
		return instantiateRepresentation( _defaultBaseRepresentation, p_target, true, false );
	}

	RepresentationManager::InstantiatedRepresentation * RepresentationManager::instantiateRepresentation(
		const Representation * const p_representation,
		Generic::BaseRepresentable & p_target,
		const bool					 p_recompute,
		const bool					 p_notify )
	{
		removeInstantiatedRepresentation( p_target, false, false );

		Model::Representation::InstantiatedRepresentation * const instantiatedRepresentation
			= MVC::MvcManager::get().instantiateModel<InstantiatedRepresentation>( p_representation );

		p_target.applyRepresentation( instantiatedRepresentation, p_recompute, p_notify );

		if ( _mapRepresentationInstances.find( p_representation ) == _mapRepresentationInstances.end() )
			_instantiateViewOnRepresentation( p_representation );

		_mapRepresentationInstances[ p_representation ].emplace( instantiatedRepresentation );

		return instantiatedRepresentation;
	}

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
				= instantiateCopy( &p_source, p_representable );

			if ( p_recompute )
				instantiatedRepresentation->getTarget()->computeAllRepresentationData();
		}
	};

	void RepresentationManager::instantiateRepresentations( const Representation * const p_representation,
															const Model::Selection &	 p_selection )
	{
		for ( const std::pair<Model::ID, Model::Selection::MapChainIds> & molData : p_selection.getItems() )
		{
			Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( molData.first );
			if ( molData.second.getFullySelectedChildCount() == molecule.getRealChainCount() )
			{
				instantiateRepresentation( p_representation, molecule, false, true );
			}
			else
			{
				for ( const std::pair<Model::ID, Model::Selection::MapResidueIds> & chainData : molData.second )
				{
					Model::Chain & chain = *molecule.getChain( chainData.first );
					if ( chainData.second.getFullySelectedChildCount() == chain.getRealResidueCount() )
					{
						instantiateRepresentation( p_representation, chain, false, true );
					}
					else
					{
						for ( const std::pair<Model::ID, Model::Selection::VecAtomIds> & residueData :
							  chainData.second )
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
		Generic::BaseRepresentable &			 p_target )
	{
		InstantiatedRepresentation * const copy
			= instantiateRepresentation( p_source->getLinkedRepresentation(), p_target, false, false );
		copy->copy( *p_source );

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

	void RepresentationManager::deleteRepresentation( const Representation *& p_representation )
	{
		_mapRepresentationInstances.erase( p_representation );
		_deleteViewOnRepresentation( p_representation );

		MVC::MvcManager::get().deleteModel( p_representation );
	}

	void RepresentationManager::_instantiateViewOnRepresentation( const Representation * const p_representation )
	{
		View::CallbackView<const Representation, RepresentationManager> * const view
			= MVC::MvcManager::get().instantiateView<View::CallbackView<const Representation, RepresentationManager>>(
				p_representation, ID::View::INSTANTIATED_REPRESENTATION_ON_REPRESENTATION );

		view->setCallback( this, &RepresentationManager::_onRepresentationChange );
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

	RepresentationManager::InstantiatedRepresentation * const RepresentationManager::getRepresentationByName(
		const std::string & p_representationName )
	{
		InstantiatedRepresentation * res = nullptr;

		for ( std::pair<InstantiatedRepresentation *, std::unordered_set<Generic::BaseRepresentable *>> item :
			  _mapRepresentablesLinkedToRepresentation )
		{
			if ( item.first->getName().compare( p_representationName ) == 0 )
			{
				res = item.first;
				break;
			}
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

} // namespace VTX::Representation
