#ifndef __VTX_REPRESENTATTION_MANAGER__
#define __VTX_REPRESENTATTION_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "generic/base_representable.hpp"
#include "model/representation/representation.hpp"
#include "model/selection.hpp"
#include <algorithm>
#include <map>
#include <set>

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

			inline void addRepresentation( const Model::Representation::BaseRepresentation * p_representation, const Generic::BaseRepresentable * p_target )
			{
				if ( _mapRepresentationOnSelections.find( p_representation ) == _mapRepresentationOnSelections.end() )
					_mapRepresentationOnSelections.emplace( p_representation, std::set<const Generic::BaseRepresentable *>() );

				_mapRepresentationOnSelections[ p_representation ].emplace( p_target );
			};
			inline void removeRepresentation( Model::Representation::BaseRepresentation * p_representation, Generic::BaseRepresentable * p_target )
			{
				_mapRepresentationOnSelections[ p_representation ].erase(
					std::find( _mapRepresentationOnSelections[ p_representation ].begin(), _mapRepresentationOnSelections[ p_representation ].end(), p_target ) );

				if ( _mapRepresentationOnSelections[ p_representation ].empty() )
					_mapRepresentationOnSelections.erase( p_representation );
			};
			inline void removeRepresentation( Model::Representation::BaseRepresentation * p_representation ) { _mapRepresentationOnSelections.erase( p_representation ); };

			inline void toggleRepresentation( const Model::Representation::BaseRepresentation * p_representation, Generic::BaseRepresentable * p_target )
			{
				if ( _mapRepresentationOnSelections.find( p_representation ) == _mapRepresentationOnSelections.end() )
					_mapRepresentationOnSelections.emplace( p_representation, std::set<const Generic::BaseRepresentable *>() );

				std::set<const Generic::BaseRepresentable *>::const_iterator representableIt = _mapRepresentationOnSelections[ p_representation ].find( p_target );
				if ( representableIt == _mapRepresentationOnSelections[ p_representation ].end() )
				{
					_mapRepresentationOnSelections[ p_representation ].emplace( p_target );
					p_target->addRepresentation( p_representation );
				}
				else
				{
					_mapRepresentationOnSelections[ p_representation ].erase( representableIt );
					p_target->removeRepresentation( p_representation );
				}
			};

			inline const std::map<const Model::Representation::BaseRepresentation *, std::set<const Generic::BaseRepresentable *>> & getRepresentations() const
			{
				return _mapRepresentationOnSelections;
			};

			inline void fillVectorWithRepresentations( const Generic::BaseRepresentable * const							p_representable,
													   std::vector<const Model::Representation::BaseRepresentation *> & p_vector ) const
			{
				for ( const std::pair<const Model::Representation::BaseRepresentation *, std::set<const Generic::BaseRepresentable *>> it : _mapRepresentationOnSelections )
				{
					if ( it.second.find( p_representable ) != it.second.cend() )
					{
						p_vector.emplace_back( it.first );
					}
				}
			};

		  private:
			std::map<const Model::Representation::BaseRepresentation *, std::set<const Generic::BaseRepresentable *>> _mapRepresentationOnSelections
				= std::map<const Model::Representation::BaseRepresentation *, std::set<const Generic::BaseRepresentable *>>();

			RepresentationManager() {};
			RepresentationManager( const RepresentationManager & ) = delete;
			RepresentationManager & operator=( const RepresentationManager & ) = delete;
			~RepresentationManager() {};
		};
	} // namespace Representation
} // namespace VTX
#endif
