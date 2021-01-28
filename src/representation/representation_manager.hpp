#ifndef __VTX_REPRESENTATION_MANAGER__
#define __VTX_REPRESENTATION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include <map>
#include <set>
#include <unordered_set>

namespace VTX
{
	namespace Generic
	{
		class BaseRepresentable;
	}
	namespace Model
	{
		class Selection;
		namespace Representation
		{
			class BaseRepresentation;
			class InstantiatedRepresentation;
		} // namespace Representation
	}	  // namespace Model

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

			inline std::set<const Model::Representation::InstantiatedRepresentation *> & getDefaultRepresentationSet()
			{
				return _defaultRepresentationSet;
			}

			inline const std::unordered_set<Generic::BaseRepresentable *> & getTargets(
				const Model::Representation::InstantiatedRepresentation * p_representation )
			{
				return _mapRepresentablesLinkedToRepresentation[ p_representation ];
			}

			void instantiateRepresentation( Model::Representation::BaseRepresentation * p_representation,
											Generic::BaseRepresentable *				p_target );
			void instantiateRepresentation( Model::Representation::BaseRepresentation * p_representation,
											const Model::Selection * const				p_selection );
			void addToRepresentation( Model::Representation::InstantiatedRepresentation * p_representation,
									  Generic::BaseRepresentable *						  p_target );
			void removeRepresentation( const Model::Representation::InstantiatedRepresentation * const p_representation,
									   Generic::BaseRepresentable *									   p_target,
									   bool															   p_update );
			void deleteRepresentation( Model::Representation::InstantiatedRepresentation * p_representation );
			void removeRepresentation( Model::Representation::BaseRepresentation * p_representation );
			const Model::Representation::InstantiatedRepresentation * const getRepresentationByName(
				const std::string & p_representationName );
			void setDefaultRepresentationIndex( const int p_defaultRepresentationIndex );

		  protected:
			RepresentationManager()
			{
				setDefaultRepresentationIndex( 0 /*VTX::Setting::REPRESENTATION_DEFAULT_INDEX*/ );
			};
			RepresentationManager( const RepresentationManager & ) = delete;
			RepresentationManager & operator=( const RepresentationManager & ) = delete;
			~RepresentationManager() {};

			void _recomputeRepresentableData( Generic::BaseRepresentable & p_representable );

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
