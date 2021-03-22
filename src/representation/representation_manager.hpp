#ifndef __VTX_REPRESENTATION_MANAGER__
#define __VTX_REPRESENTATION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include <map>
#include <set>
#include <unordered_set>

namespace VTX::Generic
{
	class BaseRepresentable;
}

namespace VTX::Model
{
	class Selection;
	namespace Representation
	{
		class BaseRepresentation;
		class InstantiatedRepresentation;
	} // namespace Representation
} // namespace VTX::Model

namespace VTX::Representation
{
	class RepresentationManager final
	{
	  public:
		using BaseRepresentation		 = Model::Representation::BaseRepresentation;
		using InstantiatedRepresentation = Model::Representation::InstantiatedRepresentation;

		using MapRepresentationRepresentables
			= std::map<const InstantiatedRepresentation *, std::unordered_set<Generic::BaseRepresentable *>>;

		inline static RepresentationManager & get()
		{
			static RepresentationManager instance;
			return instance;
		}

		void instantiateRepresentation( BaseRepresentation * p_representation, Generic::BaseRepresentable * p_target );
		void instantiateRepresentation( BaseRepresentation *		   p_representation,
										const Model::Selection * const p_selection );
		void addToRepresentation( InstantiatedRepresentation * p_representation,
								  Generic::BaseRepresentable * p_target );
		void removeRepresentation( const InstantiatedRepresentation * const p_representation,
								   Generic::BaseRepresentable *				p_target,
								   bool										p_update );
		void deleteRepresentation( InstantiatedRepresentation * p_representation );
		void removeRepresentation( BaseRepresentation * p_representation );
		const InstantiatedRepresentation * const getRepresentationByName( const std::string & p_representationName );
		void setDefaultRepresentationIndex( const int p_defaultRepresentationIndex );
		std::set<const InstantiatedRepresentation *> &			 getDefaultRepresentationSet();
		const std::unordered_set<Generic::BaseRepresentable *> & getTargets(
			const InstantiatedRepresentation * p_representation );

	  protected:
		RepresentationManager();
		RepresentationManager( const RepresentationManager & ) = delete;
		RepresentationManager & operator=( const RepresentationManager & ) = delete;
		~RepresentationManager() {};

		void _recomputeRepresentableData( Generic::BaseRepresentable & p_representable );

	  private:
		MapRepresentationRepresentables _mapRepresentablesLinkedToRepresentation = MapRepresentationRepresentables();

		std::set<const InstantiatedRepresentation *> & _defaultRepresentationSet
			= std::set<const InstantiatedRepresentation *>();
	};
} // namespace VTX::Representation
#endif
