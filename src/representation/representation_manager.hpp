#ifndef __VTX_REPRESENTATION_MANAGER__
#define __VTX_REPRESENTATION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "id.hpp"
#include <map>
#include <set>
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
			= std::map<InstantiatedRepresentation *, std::unordered_set<Generic::BaseRepresentable *>>;

		inline static RepresentationManager & get()
		{
			static RepresentationManager instance;
			return instance;
		}

		void						 setDefaultRepresentationIndex( const int p_defaultRepresentationIndex );
		InstantiatedRepresentation * instantiateDefaultRepresentation();

	  private:
		BaseRepresentation * _defaultBaseRepresentation = nullptr;

	  public:
		void instantiateRepresentation( BaseRepresentation * const	 p_representation,
										Generic::BaseRepresentable & p_target );
		void instantiateRepresentation( BaseRepresentation * const p_representation,
										const Model::Selection &   p_selection );

		void deleteRepresentation( const BaseRepresentation *& p_representation );

		InstantiatedRepresentation * const getRepresentationByName( const std::string & p_representationName );
		const std::unordered_set<Generic::BaseRepresentable *> & getTargets(
			InstantiatedRepresentation * p_representation );

	  protected:
		RepresentationManager();
		RepresentationManager( const RepresentationManager & ) = delete;
		RepresentationManager & operator=( const RepresentationManager & ) = delete;
		~RepresentationManager() {};

	  private:
		using MoleculeRepresentationData
			= std::map<Generic::BaseRepresentable *, std::set<InstantiatedRepresentation *>>;

		MapRepresentationRepresentables _mapRepresentablesLinkedToRepresentation = MapRepresentationRepresentables();
		std::set<InstantiatedRepresentation *> & _defaultRepresentationSet = std::set<InstantiatedRepresentation *>();
	};
} // namespace VTX::Representation
#endif
