#ifndef __VTX_REPRESENTATION_MANAGER__
#define __VTX_REPRESENTATION_MANAGER__

#include "app/event/vtx_event.hpp"
#include "app/old_app/id.hpp"
#include "app/old_app/model/molecule.hpp"
#include "app/old_app/model/representation/representation_data.hpp"
#include "app/old_app/model/representation/representation_enum.hpp"
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
		using PairRepresentationRepresentables
			= std::pair<InstantiatedRepresentation * const, std::unordered_set<Generic::BaseRepresentable *>>;

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
		void assignRepresentation( InstantiatedRepresentation * const p_instantiatedRepresentation,
								   Generic::BaseRepresentable &		  p_target,
								   const bool						  p_recompute = true,
								   const bool						  p_notify	  = true );

		RepresentationManager::InstantiatedRepresentation * instantiateCopy(
			const InstantiatedRepresentation * const p_source,
			Generic::BaseRepresentable &			 p_target,
			const bool								 p_recompute = true,
			const bool								 p_notify	 = true );
		void instantiateRepresentations( const Representation * const p_representation,
										 const Model::Selection &	  p_selection );
		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseRepresentable, T>::value>>
		void instantiateRepresentations( const Representation * const	 p_representation,
										 const std::unordered_set<T *> & p_targets )
		{
			std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

			for ( T * const representable : p_targets )
			{
				instantiateRepresentation( p_representation, *representable, false );
				molecules.emplace( representable->getMolecule() );
			}

			for ( Model::Molecule * const molecule : molecules )
			{
				molecule->computeAllRepresentationData();
			}
		}

		InstantiatedRepresentation * instantiateDefaultRepresentation( Generic::BaseRepresentable & p_target,
																	   const bool					p_recompute = true,
																	   const bool					p_notify = true );

		void applyRepresentation( Generic::BaseRepresentable &				 p_representable,
								  const InstantiatedRepresentation &		 p_source,
								  const Model::Representation::MEMBER_FLAG & p_flag
								  = Model::Representation::MEMBER_FLAG::ALL,
								  const bool p_recompute = true );

		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseRepresentable, T>::value>>
		void applyRepresentation( std::unordered_set<T *>					 p_representables,
								  const InstantiatedRepresentation &		 p_source,
								  const Model::Representation::MEMBER_FLAG & p_flag
								  = Model::Representation::MEMBER_FLAG::ALL )
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

			if ( _mapRepresentationInstances[ representation ].size() == 0 )
				_mapRepresentationInstances.erase( representation );

			p_representable.removeRepresentation( p_notify );

			if ( p_recompute )
				p_representable.getMolecule()->computeAllRepresentationData();
		}
		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseRepresentable, T>::value>>
		void removeInstantiatedRepresentations( const std::unordered_set<T *> & p_targets )
		{
			std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();

			for ( T * const representable : p_targets )
			{
				removeInstantiatedRepresentation( *representable, false, true );
				molecules.emplace( representable->getMolecule() );
			}

			for ( Model::Molecule * const molecule : molecules )
			{
				molecule->computeAllRepresentationData();
			}
		}

		void clearAllRepresentations( const bool p_notify = true );

		void deleteRepresentation( const Representation * const p_representation );
		void setQuickAccessToPreset( Representation * const p_representation, const bool p_quickAccess );

		InstantiatedRepresentation * const getRepresentationByName( const std::string & p_representationName ) const;
		const std::unordered_set<Generic::BaseRepresentable *> & getTargets(
			InstantiatedRepresentation * p_representation );
		const std::unordered_set<InstantiatedRepresentation *> getAllInstantiatedRepresentations(
			const Representation * const p_representation )
		{
			return _mapRepresentationInstances[ p_representation ];
		}
		InstantiatedRepresentation * instantiateDummy( const InstantiatedRepresentation & p_source );

		void storeRepresentations();
		void restoreRepresentations();

		int	 getDefaultRepresentationIndex() const;
		void resetRepresentations();

	  protected:
		RepresentationManager();
		RepresentationManager( const RepresentationManager & )			   = delete;
		RepresentationManager & operator=( const RepresentationManager & ) = delete;
		~RepresentationManager() {};

	  private:
		using MoleculeRepresentationData
			= std::map<Generic::BaseRepresentable *, std::unordered_set<InstantiatedRepresentation *>>;

		using MapRepresentationInstances
			= std::map<const Model::Representation::Representation *, std::unordered_set<InstantiatedRepresentation *>>;
		using PairRepresentationInstances = std::pair<const Model::Representation::Representation * const,
													  std::unordered_set<InstantiatedRepresentation *>>;

		using MapStoredRepresentation = std::map<std::string, const std::unordered_set<InstantiatedRepresentation *>>;
		using PairStoredRepresentation
			= std::pair<const std::string, const std::unordered_set<InstantiatedRepresentation *>>;

		MapRepresentationInstances		_mapRepresentationInstances				 = MapRepresentationInstances();
		MapRepresentationRepresentables _mapRepresentablesLinkedToRepresentation = MapRepresentationRepresentables();

		Representation *		_lastRepresentationQuickAccessed = nullptr;
		MapStoredRepresentation _storedRepresentations;

		void _instantiateViewOnRepresentation( const Representation * const p_representation );
		void _deleteViewOnRepresentation( const Representation * const p_representation ) const;

		void _applyNextBaseRepresentationOnInstances( const Representation * const p_defaultRepresentation );

		void _onRepresentationChange( const Representation * const	representation,
									  const Event::VTXEvent * const p_event );
		int	 _getRepresentationWithQuickAccessCount() const;
	};
} // namespace VTX::Representation
#endif
