#ifndef __VTX_REPRESENTATION_MANAGER__
#define __VTX_REPRESENTATION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "generic/base_representable.hpp"
#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/selection.hpp"
//#include "setting.hpp"
#include <algorithm>
#include <set>
#include <string>
#include <vector>

namespace VTX
{
	namespace Representation
	{
		class RepresentationManager final
		{
		  public:
			inline static RepresentationManager & get()
			{
				static RepresentationManager instance;
				return instance;
			}

			inline void instantiateRepresentation( Model::Representation::BaseRepresentation * p_representation,
												   Generic::BaseRepresentable *				   p_target )
			{
				Model::Representation::InstantiatedRepresentation * instantiatedRepresentation
					= MVC::MvcManager::get().instantiateModel<Model::Representation::InstantiatedRepresentation>(
						p_representation );

				p_target->addRepresentation( instantiatedRepresentation );

				_mapRepresentablesLinkedToRepresentation.emplace( instantiatedRepresentation,
																  std::unordered_set<Generic::BaseRepresentable *>() );
				_mapRepresentablesLinkedToRepresentation[ instantiatedRepresentation ].emplace( p_target );

				_recomputeRepresentableData( *p_target );

				VTX_EVENT( new Event::VTXEventPtr<Model::Representation::InstantiatedRepresentation>(
					Event::REPRESENTATION_ADDED, instantiatedRepresentation ) );
			};
			inline void instantiateRepresentation( Model::Representation::BaseRepresentation * p_representation,
												   const Model::Selection * const			   p_selection )
			{
				std::set<Model::Molecule *> molecules = std::set<Model::Molecule *>();

				Model::Representation::InstantiatedRepresentation * instantiatedRepresentation
					= MVC::MvcManager::get().instantiateModel<Model::Representation::InstantiatedRepresentation>(
						p_representation );
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

				VTX_EVENT( new Event::VTXEventPtr<Model::Representation::InstantiatedRepresentation>(
					Event::REPRESENTATION_ADDED, instantiatedRepresentation ) );
			};

			inline void addToRepresentation( Model::Representation::InstantiatedRepresentation * p_representation,
											 Generic::BaseRepresentable *						 p_target )
			{
				p_target->addRepresentation( p_representation );
				_recomputeRepresentableData( *p_target );

				_mapRepresentablesLinkedToRepresentation[ p_representation ].emplace( p_target );
			};

			inline void removeRepresentation(
				const Model::Representation::InstantiatedRepresentation * const p_representation,
				Generic::BaseRepresentable *									p_target,
				bool															p_update )
			{
				_mapRepresentablesLinkedToRepresentation[ p_representation ].erase( p_target );
				p_target->removeRepresentation( p_representation );

				if ( p_update )
					_recomputeRepresentableData( *p_target );
			};
			inline void deleteRepresentation( Model::Representation::InstantiatedRepresentation * p_representation )
			{
				std::unordered_map<const Model::Molecule *, Generic::BaseRepresentable *> molecules
					= std::unordered_map<const Model::Molecule *, Generic::BaseRepresentable *>();
				for ( Generic::BaseRepresentable * target :
					  _mapRepresentablesLinkedToRepresentation[ p_representation ] )
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
					Event::REPRESENTATION_REMOVED, p_representation ) );

				delete p_representation;
				p_representation = nullptr;
			};
			inline void removeRepresentation( Model::Representation::BaseRepresentation * p_representation ) {
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

			inline std::set<const Model::Representation::InstantiatedRepresentation *> & getDefaultRepresentationSet()
			{
				return _defaultRepresentationSet;
			};

			const std::unordered_set<Generic::BaseRepresentable *> & getTargets(
				const Model::Representation::InstantiatedRepresentation * p_representation )
			{
				return _mapRepresentablesLinkedToRepresentation[ p_representation ];
			}

			const Model::Representation::InstantiatedRepresentation * const getRepresentationByName(
				const std::string & p_representationName )
			{
				const Model::Representation::InstantiatedRepresentation * res = nullptr;

				for ( std::pair<const Model::Representation::InstantiatedRepresentation *,
								std::unordered_set<Generic::BaseRepresentable *>> item :
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

			inline void setDefaultRepresentationIndex( const int p_defaultRepresentationIndex )
			{
				Model::Representation::BaseRepresentation * const defaultRep
					= Model::Representation::RepresentationLibrary::get().getRepresentation(
						p_defaultRepresentationIndex );

				Model::Representation::InstantiatedRepresentation * instantiatedDefautRep
					= MVC::MvcManager::get().instantiateModel<Model::Representation::InstantiatedRepresentation>(
						defaultRep );

				_defaultRepresentationSet.clear();
				_defaultRepresentationSet.emplace( instantiatedDefautRep );
			}

		  protected:
			RepresentationManager()
			{
				setDefaultRepresentationIndex( 0 /*VTX::Setting::REPRESENTATION_DEFAULT_INDEX*/ );
			};
			RepresentationManager( const RepresentationManager & ) = delete;
			RepresentationManager & operator=( const RepresentationManager & ) = delete;
			~RepresentationManager() {};

			void _recomputeRepresentableData( Generic::BaseRepresentable & p_representable )
			{
				p_representable.computeRepresentationTargets();
				p_representable.computeColorBuffer();
			}

		  private:
			std::map<const Model::Representation::InstantiatedRepresentation *,
					 std::unordered_set<Generic::BaseRepresentable *>>
				_mapRepresentablesLinkedToRepresentation
				= std::map<const Model::Representation::InstantiatedRepresentation *,
						   std::unordered_set<Generic::BaseRepresentable *>>();

			std::set<const Model::Representation::InstantiatedRepresentation *> & _defaultRepresentationSet
				= std::set<const Model::Representation::InstantiatedRepresentation *>();
		};
	} // namespace Representation
} // namespace VTX
#endif
