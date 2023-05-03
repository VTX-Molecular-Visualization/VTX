#ifndef __VTX_APPLICATION_REPRESENTATION_REPRESENTATION_MANAGER__
#define __VTX_APPLICATION_REPRESENTATION_REPRESENTATION_MANAGER__

#include "app/application/representation/_fwd.hpp"
#include "app/application/representation/enum_representation.hpp"
#include "app/application/representation/representation_data.hpp"
#include "app/application/selection/_fwd.hpp"
#include "app/component/chemistry/_fwd.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/core/event/vtx_event.hpp"
#include "app/id.hpp"
#include <map>
#include <unordered_set>

namespace VTX::App::Application::Representation
{
	class RepresentationManager final
	{
	  public:
		using MapRepresentationRepresentables
			= std::map<InstantiatedRepresentation *, std::unordered_set<BaseRepresentable *>>;
		using PairRepresentationRepresentables
			= std::pair<InstantiatedRepresentation * const, std::unordered_set<BaseRepresentable *>>;

		inline static RepresentationManager & get()
		{
			static RepresentationManager instance;
			return instance;
		}

	  public:
		InstantiatedRepresentation * instantiateRepresentation( const RepresentationPreset * const p_representation,
																BaseRepresentable &				   p_target,
																const bool						   p_recompute = true,
																const bool						   p_notify	   = true );
		void assignRepresentation( InstantiatedRepresentation * const p_instantiatedRepresentation,
								   BaseRepresentable &				  p_target,
								   const bool						  p_recompute = true,
								   const bool						  p_notify	  = true );

		InstantiatedRepresentation * instantiateCopy( const InstantiatedRepresentation * const p_source,
													  BaseRepresentable &					   p_target,
													  const bool							   p_recompute = true,
													  const bool							   p_notify	   = true );
		void						 instantiateRepresentations( const RepresentationPreset * const				p_representation,
																 const App::Application::Selection::SelectionModel & p_selection );
		template<typename T, typename = std::enable_if<std::is_base_of<BaseRepresentable, T>::value>>
		void instantiateRepresentations( const RepresentationPreset * const p_representation,
										 const std::unordered_set<T *> &	p_targets )
		{
			std::unordered_set<App::Component::Chemistry::Molecule *> molecules
				= std::unordered_set<App::Component::Chemistry::Molecule *>();

			for ( T * const representable : p_targets )
			{
				instantiateRepresentation( p_representation, *representable, false );
				molecules.emplace( representable->getMolecule() );
			}

			for ( App::Component::Chemistry::Molecule * const molecule : molecules )
			{
				molecule->computeAllRepresentationData();
			}
		}

		InstantiatedRepresentation * instantiateDefaultRepresentation( BaseRepresentable & p_target,
																	   const bool		   p_recompute = true,
																	   const bool		   p_notify	   = true );

		void applyRepresentation( BaseRepresentable &				 p_representable,
								  const InstantiatedRepresentation & p_source,
								  const MEMBER_FLAG &				 p_flag		 = MEMBER_FLAG::ENUM::ALL,
								  const bool						 p_recompute = true );

		template<typename T, typename = std::enable_if<std::is_base_of<BaseRepresentable, T>::value>>
		void applyRepresentation( std::unordered_set<T *>			 p_representables,
								  const InstantiatedRepresentation & p_source,
								  const MEMBER_FLAG &				 p_flag = MEMBER_FLAG::ENUM::ALL )
		{
			std::unordered_set<App::Component::Chemistry::Molecule *> molecules
				= std::unordered_set<App::Component::Chemistry::Molecule *>();

			for ( T * representable : p_representables )
			{
				applyRepresentation( *representable, p_source, p_flag, false );
				molecules.emplace( representable->getMolecule() );
			}

			for ( App::Component::Chemistry::Molecule * const molecule : molecules )
			{
				molecule->computeAllRepresentationData();
			}
		};

		template<typename T, typename = std::enable_if<std::is_base_of<BaseRepresentable, T>::value>>
		void removeInstantiatedRepresentation( T &		  p_representable,
											   const bool p_recompute = true,
											   const bool p_notify	  = true )
		{
			if ( !p_representable.hasCustomRepresentation() )
				return;

			InstantiatedRepresentation * const instantiatedRepresentation = p_representable.getCustomRepresentation();
			const RepresentationPreset * const representation = instantiatedRepresentation->getLinkedRepresentation();

			_mapRepresentationInstances[ representation ].erase( instantiatedRepresentation );

			if ( _mapRepresentationInstances[ representation ].size() == 0 )
				_mapRepresentationInstances.erase( representation );

			p_representable.removeRepresentation( p_notify );

			if ( p_recompute )
				p_representable.getMolecule()->computeAllRepresentationData();
		}
		template<typename T, typename = std::enable_if<std::is_base_of<BaseRepresentable, T>::value>>
		void removeInstantiatedRepresentations( const std::unordered_set<T *> & p_targets )
		{
			std::unordered_set<App::Component::Chemistry::Molecule *> molecules
				= std::unordered_set<App::Component::Chemistry::Molecule *>();

			for ( T * const representable : p_targets )
			{
				removeInstantiatedRepresentation( *representable, false, true );
				molecules.emplace( representable->getMolecule() );
			}

			for ( App::Component::Chemistry::Molecule * const molecule : molecules )
			{
				molecule->computeAllRepresentationData();
			}
		}

		void clearAllRepresentations( const bool p_notify = true );

		void deleteRepresentation( const RepresentationPreset * const p_representation );
		void setQuickAccessToPreset( RepresentationPreset * const p_representation, const bool p_quickAccess );

		InstantiatedRepresentation * const getRepresentationByName( const std::string & p_representationName ) const;
		const std::unordered_set<BaseRepresentable *> & getTargets( InstantiatedRepresentation * p_representation );
		const std::unordered_set<InstantiatedRepresentation *> getAllInstantiatedRepresentations(
			const RepresentationPreset * const p_representation )
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
			= std::map<BaseRepresentable *, std::unordered_set<InstantiatedRepresentation *>>;

		using MapRepresentationInstances
			= std::map<const RepresentationPreset *, std::unordered_set<InstantiatedRepresentation *>>;
		using PairRepresentationInstances
			= std::pair<const RepresentationPreset * const, std::unordered_set<InstantiatedRepresentation *>>;

		using MapStoredRepresentation = std::map<std::string, const std::unordered_set<InstantiatedRepresentation *>>;
		using PairStoredRepresentation
			= std::pair<const std::string, const std::unordered_set<InstantiatedRepresentation *>>;

		MapRepresentationInstances		_mapRepresentationInstances				 = MapRepresentationInstances();
		MapRepresentationRepresentables _mapRepresentablesLinkedToRepresentation = MapRepresentationRepresentables();

		RepresentationPreset *	_lastRepresentationQuickAccessed = nullptr;
		MapStoredRepresentation _storedRepresentations;

		void _instantiateViewOnRepresentation( const RepresentationPreset * const p_representation );
		void _deleteViewOnRepresentation( const RepresentationPreset * const p_representation ) const;

		void _applyNextBaseRepresentationOnInstances( const RepresentationPreset * const p_defaultRepresentation );

		void _onRepresentationChange( const RepresentationPreset * const	   representation,
									  const App::Core::Event::VTXEvent * const p_event );
		int	 _getRepresentationWithQuickAccessCount() const;
	};
} // namespace VTX::App::Application::Representation
#endif
