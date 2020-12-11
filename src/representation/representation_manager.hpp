#ifndef __VTX_REPRESENTATION_MANAGER__
#define __VTX_REPRESENTATION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "generic/base_representable.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/selection.hpp"
//#include "setting.hpp"
#include <algorithm>
#include <set>
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

			inline void instantiateRepresentation( Model::Representation::BaseRepresentation * p_representation, Generic::BaseRepresentable * p_target )
			{
				p_target->addRepresentation( p_representation );
				p_target->computeRepresentationTargets();

				if ( _mapRepresentablesLinkedToRepresentation.find( p_representation ) == _mapRepresentablesLinkedToRepresentation.end() )
					_mapRepresentablesLinkedToRepresentation.emplace( p_representation, std::unordered_set<Generic::BaseRepresentable *>() );

				_mapRepresentablesLinkedToRepresentation[ p_representation ].emplace( p_target );

				VTX_EVENT( new Event::VTXEventPtr<Model::Representation::BaseRepresentation>( Event::REPRESENTATION_ADDED, p_representation ) );
			};
			inline void instantiateRepresentation( Model::Representation::BaseRepresentation * p_representation, const Model::Selection * const p_selection )
			{
				std::set<Model::Molecule *> molecules = std::set<Model::Molecule *>();

				if ( _mapRepresentablesLinkedToRepresentation.find( p_representation ) == _mapRepresentablesLinkedToRepresentation.end() )
					_mapRepresentablesLinkedToRepresentation.emplace( p_representation, std::unordered_set<Generic::BaseRepresentable *>() );

				for ( const std::pair<Model::ID, Model::Selection::MapChainIds> mapMolecule : p_selection->getItems() )
				{
					Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( mapMolecule.first );
					for ( const std::pair<Model::ID, Model::Selection::MapResidueIds> mapChain : mapMolecule.second )
					{
						for ( const std::pair<Model::ID, Model::Selection::VecAtomIds> mapResidue : mapChain.second )
						{
							Model::Residue & residue = molecule.getResidue( mapResidue.first );
							residue.addRepresentation( p_representation );

							_mapRepresentablesLinkedToRepresentation[ p_representation ].emplace( &residue );
						}
					}

					molecule.computeRepresentationTargets();
				}

				VTX_EVENT( new Event::VTXEventPtr<Model::Representation::BaseRepresentation>( Event::REPRESENTATION_ADDED, p_representation ) );
			};

			inline void addRepresentation( Model::Representation::BaseRepresentation * p_representation, Generic::BaseRepresentable * p_target )
			{
				p_target->addRepresentation( p_representation );
				p_target->computeRepresentationTargets();

				if ( _mapRepresentablesLinkedToRepresentation.find( p_representation ) == _mapRepresentablesLinkedToRepresentation.end() )
					_mapRepresentablesLinkedToRepresentation.emplace( p_representation, std::unordered_set<Generic::BaseRepresentable *>() );

				_mapRepresentablesLinkedToRepresentation[ p_representation ].emplace( p_target );
			};

			inline void removeRepresentation( const Model::Representation::BaseRepresentation * p_representation, Generic::BaseRepresentable * p_target )
			{
				_mapRepresentablesLinkedToRepresentation[ p_representation ].erase( p_target );
				p_target->removeRepresentation( p_representation );

				p_target->computeRepresentationTargets();
			};
			inline void removeRepresentation( Model::Representation::BaseRepresentation * p_representation )
			{
				std::unordered_map<const Model::Molecule *, Generic::BaseRepresentable *> molecules = std::unordered_map<const Model::Molecule *, Generic::BaseRepresentable *>();
				for ( Generic::BaseRepresentable * target : _mapRepresentablesLinkedToRepresentation[ p_representation ] )
				{
					target->removeRepresentation( p_representation );
					molecules.emplace( target->getMolecule(), target );
				}

				for ( std::pair<const Model::Molecule *, Generic::BaseRepresentable *> pairMoleculeRep : molecules )
				{
					pairMoleculeRep.second->computeRepresentationTargets();
				}

				_mapRepresentablesLinkedToRepresentation.erase( p_representation );

				VTX_EVENT( new Event::VTXEventPtr<Model::Representation::BaseRepresentation>( Event::REPRESENTATION_REMOVED, p_representation ) );
			};

			inline void toggleRepresentation( const Model::Representation::BaseRepresentation * p_representation, Generic::BaseRepresentable * p_target )
			{
				p_target->toggleRepresentation( p_representation );
			};
			inline void toggleRepresentation( const Model::Representation::BaseRepresentation * p_representation, const Model::Selection * const p_selection )
			{
				for ( const std::pair<Model::ID, Model::Selection::MapChainIds> mapMolecule : p_selection->getItems() )
				{
					Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( mapMolecule.first );
					for ( const std::pair<Model::ID, Model::Selection::MapResidueIds> mapChain : mapMolecule.second )
					{
						for ( const std::pair<Model::ID, Model::Selection::VecAtomIds> mapResidue : mapChain.second )
						{
							Model::Residue & residue = molecule.getResidue( mapResidue.first );
							residue.toggleRepresentation( p_representation );
						}
					}

					molecule.computeRepresentationTargets();
				}
			};

			inline std::set<const Model::Representation::BaseRepresentation *> & getDefaultRepresentationSet() { return _defaultRepresentationSet; };

			const std::unordered_set<Generic::BaseRepresentable *> getTargets( const Model::Representation::BaseRepresentation * p_representation )
			{
				return _mapRepresentablesLinkedToRepresentation[ p_representation ];
			}

			inline void setDefaultRepresentationIndex( const int p_defaultRepresentationIndex )
			{
				const Model::Representation::BaseRepresentation * const defaultRep
					= Model::Representation::RepresentationLibrary::get().getRepresentation( p_defaultRepresentationIndex );

				_defaultRepresentationSet.clear();
				_defaultRepresentationSet.emplace( defaultRep );
			}

		  private:
			RepresentationManager() { setDefaultRepresentationIndex( 0 /*VTX::Setting::REPRESENTATION_DEFAULT_INDEX*/ ); };
			RepresentationManager( const RepresentationManager & ) = delete;
			RepresentationManager & operator=( const RepresentationManager & ) = delete;
			~RepresentationManager() {};

			std::map<const Model::Representation::BaseRepresentation *, std::unordered_set<Generic::BaseRepresentable *>> _mapRepresentablesLinkedToRepresentation
				= std::map<const Model::Representation::BaseRepresentation *, std::unordered_set<Generic::BaseRepresentable *>>();

			std::set<const Model::Representation::BaseRepresentation *> & _defaultRepresentationSet = std::set<const Model::Representation::BaseRepresentation *>();
		};
	} // namespace Representation
} // namespace VTX
#endif
