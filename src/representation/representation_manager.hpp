#ifndef __VTX_REPRESENTATION_MANAGER__
#define __VTX_REPRESENTATION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
#include "id.hpp"
#include "model/representation/representation_data.hpp"
#include "model/representation/representation_enum.hpp"
#include <map>
#include <unordered_set>

namespace VTX::Generic
{
	class BaseRepresentable;
}

namespace VTX::Model
{
	class Molecule;
	class Selection;
	namespace Representation
	{
		class Representation;
		class InstantiatedRepresentation;
		class RepresentationLibrary;
	} // namespace Representation
} // namespace VTX::Model

namespace VTX::Representation
{
	class RepresentationManager final
	{
	  public:
		using Representation			 = Model::Representation::Representation;
		using InstantiatedRepresentation = Model::Representation::InstantiatedRepresentation;
		using RepresentationLibrary		 = Model::Representation::RepresentationLibrary;

		using MapRepresentationRepresentables
			= std::map<InstantiatedRepresentation *, std::unordered_set<Generic::BaseRepresentable *>>;

		inline static RepresentationManager & get()
		{
			static RepresentationManager instance;
			return instance;
		}

	  public:
		InstantiatedRepresentation * instantiateRepresentation( const Representation * const p_representation,
																Generic::BaseRepresentable & p_target,
																const bool					 p_recompute = true,
																const bool					 p_notify	 = true );
		RepresentationManager::InstantiatedRepresentation * instantiateCopy(
			const InstantiatedRepresentation * const p_source,
			Generic::BaseRepresentable &			 p_target );
		void instantiateRepresentations( const Representation * const p_representation,
										 const Model::Selection &	  p_selection );
		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseRepresentable, T>::value>>
		void instantiateRepresentations( const Representation * const p_representation,
										 std::unordered_set<T *> &	  p_targets )
		{
			std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

			for ( T * representable : p_targets )
			{
				instantiateRepresentation( p_representation, *representable, false );
				molecules.emplace( representable->getMolecule() );
			}

			for ( Model::Molecule * const molecule : molecules )
			{
				molecule->computeAllRepresentationData();
			}
		}

		InstantiatedRepresentation * instantiateDefaultRepresentation( Generic::BaseRepresentable & p_target );

		void applyRepresentation( Generic::BaseRepresentable &				 p_representable,
								  const InstantiatedRepresentation &		 p_source,
								  const Model::Representation::MEMBER_FLAG & p_flag,
								  const bool								 p_recompute = true );

		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseRepresentable, T>::value>>
		void applyRepresentation( std::unordered_set<T *>					 p_representables,
								  const InstantiatedRepresentation &		 p_source,
								  const Model::Representation::MEMBER_FLAG & p_flag )
		{
			std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

			for ( T * representable : p_representables )
			{
				applyRepresentation( *representable, p_source, p_flag, false );
				molecules.emplace( representable->getMolecule() );
			}

			for ( Model::Molecule * const molecule : molecules )
			{
				molecule->computeAllRepresentationData();
			}
		};

		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseRepresentable, T>::value>>
		void removeInstantiatedRepresentation( T &		  p_representable,
											   const bool p_recompute = true,
											   const bool p_notify	  = true )
		{
			if ( !p_representable.hasCustomRepresentation() )
				return;

			InstantiatedRepresentation * const instantiatedRepresentation = p_representable.getCustomRepresentation();
			const Representation * const	   representation = instantiatedRepresentation->getLinkedRepresentation();

			_mapRepresentationInstances[ representation ].erase( instantiatedRepresentation );

			p_representable.removeRepresentation( p_notify );

			if ( p_recompute )
				p_representable.getMolecule()->computeAllRepresentationData();
		}
		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseRepresentable, T>::value>>
		void removeInstantiatedRepresentations( std::unordered_set<T *> & p_targets )
		{
			std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

			for ( T * representable : p_targets )
			{
				removeInstantiatedRepresentation( *representable, false, true );
				molecules.emplace( representable->getMolecule() );
			}

			for ( Model::Molecule * const molecule : molecules )
			{
				molecule->computeAllRepresentationData();
			}
		}

		void deleteRepresentation( const Representation *& p_representation );

		InstantiatedRepresentation * const getRepresentationByName( const std::string & p_representationName );
		const std::unordered_set<Generic::BaseRepresentable *> & getTargets(
			InstantiatedRepresentation * p_representation );
		InstantiatedRepresentation * instantiateDummy( const InstantiatedRepresentation & p_source );

	  protected:
		RepresentationManager();
		RepresentationManager( const RepresentationManager & ) = delete;
		RepresentationManager & operator=( const RepresentationManager & ) = delete;
		~RepresentationManager() {};

	  private:
		using MoleculeRepresentationData
			= std::map<Generic::BaseRepresentable *, std::unordered_set<InstantiatedRepresentation *>>;
		using MapRepresentationInstances
			= std::map<const Model::Representation::Representation *, std::unordered_set<InstantiatedRepresentation *>>;

		MapRepresentationInstances		_mapRepresentationInstances				 = MapRepresentationInstances();
		MapRepresentationRepresentables _mapRepresentablesLinkedToRepresentation = MapRepresentationRepresentables();

		void _instantiateViewOnRepresentation( const Representation * const p_representation );
		void _deleteViewOnRepresentation( const Representation * const p_representation ) const;

		void _applyNextBaseRepresentationOnInstances( const Representation * const p_defaultRepresentation );

		void _onRepresentationChange( const Representation * const	representation,
									  const Event::VTXEvent * const p_event );
	};
} // namespace VTX::Representation
#endif
