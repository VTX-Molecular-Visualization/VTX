#include "representation_manager.hpp"
#include "event/event_manager.hpp"
#include "generic/base_representable.hpp"
#include "model/molecule.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/residue.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "setting.hpp"

namespace VTX
{
	namespace Representation
	{
		RepresentationManager::RepresentationManager()
		{
			setDefaultRepresentationIndex( VTX::Setting::REPRESENTATION_DEFAULT_INDEX );
		}

		void RepresentationManager::instantiateRepresentation( BaseRepresentation *			p_representation,
															   Generic::BaseRepresentable * p_target )
		{
			InstantiatedRepresentation * instantiatedRepresentation
				= MVC::MvcManager::get().instantiateModel<InstantiatedRepresentation>( p_representation );

			p_target->addRepresentation( instantiatedRepresentation );

			_mapRepresentablesLinkedToRepresentation.emplace( instantiatedRepresentation,
															  std::unordered_set<Generic::BaseRepresentable *>() );
			_mapRepresentablesLinkedToRepresentation[ instantiatedRepresentation ].emplace( p_target );

			_recomputeRepresentableData( *p_target );

			VTX_EVENT( new Event::VTXEventPtr<InstantiatedRepresentation>( Event::REPRESENTATION_ADDED,
																		   instantiatedRepresentation ) );
		};
		void RepresentationManager::instantiateRepresentation( BaseRepresentation *			  p_representation,
															   const Model::Selection * const p_selection )
		{
			std::set<Model::Molecule *> molecules = std::set<Model::Molecule *>();

			InstantiatedRepresentation * instantiatedRepresentation
				= MVC::MvcManager::get().instantiateModel<InstantiatedRepresentation>( p_representation );
			_mapRepresentablesLinkedToRepresentation.emplace( instantiatedRepresentation,
															  std::unordered_set<Generic::BaseRepresentable *>() );

			for ( const std::pair<Model::ID, Model::Selection::MapChainIds> mapMolecule : p_selection->getItems() )
			{
				Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( mapMolecule.first );
				for ( const std::pair<Model::ID, Model::Selection::MapResidueIds> mapChain : mapMolecule.second )
				{
					for ( const std::pair<Model::ID, Model::Selection::VecAtomIds> mapResidue : mapChain.second )
					{
						Model::Residue * residue = molecule.getResidue( mapResidue.first );
						residue->addRepresentation( instantiatedRepresentation );

						_mapRepresentablesLinkedToRepresentation[ instantiatedRepresentation ].emplace( residue );
					}
				}

				_recomputeRepresentableData( molecule );
			}

			VTX_EVENT( new Event::VTXEventPtr<InstantiatedRepresentation>( Event::REPRESENTATION_ADDED,
																		   instantiatedRepresentation ) );
		};

		void RepresentationManager::addToRepresentation( InstantiatedRepresentation * p_representation,
														 Generic::BaseRepresentable * p_target )
		{
			p_target->addRepresentation( p_representation );
			_recomputeRepresentableData( *p_target );

			_mapRepresentablesLinkedToRepresentation[ p_representation ].emplace( p_target );
		};

		void RepresentationManager::removeRepresentation( const InstantiatedRepresentation * const p_representation,
														  Generic::BaseRepresentable *			   p_target,
														  bool									   p_update )
		{
			_mapRepresentablesLinkedToRepresentation[ p_representation ].erase( p_target );
			p_target->removeRepresentation( p_representation );

			if ( p_update )
				_recomputeRepresentableData( *p_target );
		};
		void RepresentationManager::deleteRepresentation( InstantiatedRepresentation * p_representation )
		{
			std::unordered_map<const Model::Molecule *, Generic::BaseRepresentable *> molecules
				= std::unordered_map<const Model::Molecule *, Generic::BaseRepresentable *>();
			for ( Generic::BaseRepresentable * target : _mapRepresentablesLinkedToRepresentation[ p_representation ] )
			{
				target->removeRepresentation( p_representation );
				molecules.emplace( target->getMolecule(), target );
			}

			for ( std::pair<const Model::Molecule *, Generic::BaseRepresentable *> pairMoleculeRep : molecules )
			{
				_recomputeRepresentableData( *( pairMoleculeRep.second ) );
			}

			_mapRepresentablesLinkedToRepresentation.erase( p_representation );

			VTX_EVENT(
				new Event::VTXEventPtr<InstantiatedRepresentation>( Event::REPRESENTATION_REMOVED, p_representation ) );

			delete p_representation;
			p_representation = nullptr;
		};
		void RepresentationManager::removeRepresentation(
			Model::Representation::BaseRepresentation * p_representation ) {
			/*std::unordered_map<const Model::Molecule *, Generic::BaseRepresentable *> molecules =
			std::unordered_map<const Model::Molecule *, Generic::BaseRepresentable *>(); for (
			Generic::BaseRepresentable * target : _mapRepresentablesLinkedToRepresentation[ p_representation ] )
			{
				target->removeRepresentation( p_representation );
				molecules.emplace( target->getMolecule(), target );
			}

			for ( std::pair<const Model::Molecule *, Generic::BaseRepresentable *> pairMoleculeRep : molecules )
			{
				_recomputeRepresentableData( *( pairMoleculeRep.second ) );
			}

			_mapRepresentablesLinkedToRepresentation.erase( p_representation );

			VTX_EVENT( new Event::VTXEventPtr<Model::Representation::InstantiatedRepresentation>(
			Event::REPRESENTATION_REMOVED, p_representation ) );*/
		};

		std::set<const Model::Representation::InstantiatedRepresentation *> & RepresentationManager::
			getDefaultRepresentationSet()
		{
			return _defaultRepresentationSet;
		};

		const std::unordered_set<Generic::BaseRepresentable *> & RepresentationManager::getTargets(
			const InstantiatedRepresentation * p_representation )
		{
			return _mapRepresentablesLinkedToRepresentation[ p_representation ];
		}

		const Model::Representation::InstantiatedRepresentation * const RepresentationManager::getRepresentationByName(
			const std::string & p_representationName )
		{
			const InstantiatedRepresentation * res = nullptr;

			for ( std::pair<const InstantiatedRepresentation *, std::unordered_set<Generic::BaseRepresentable *>> item :
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

		void RepresentationManager::setDefaultRepresentationIndex( const int p_defaultRepresentationIndex )
		{
			BaseRepresentation * const defaultRep
				= Model::Representation::RepresentationLibrary::get().getRepresentation( p_defaultRepresentationIndex );

			InstantiatedRepresentation * instantiatedDefautRep
				= MVC::MvcManager::get().instantiateModel<InstantiatedRepresentation>( defaultRep );

			_defaultRepresentationSet.clear();
			_defaultRepresentationSet.emplace( instantiatedDefautRep );
		}

		void RepresentationManager::_recomputeRepresentableData( Generic::BaseRepresentable & p_representable )
		{
			p_representable.computeRepresentationTargets();
			p_representable.computeColorBuffer();
		}

	} // namespace Representation
} // namespace VTX
