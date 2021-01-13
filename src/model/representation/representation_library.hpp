#ifndef __VTX_MODEL_REPRESENTATION_LIBRARY__
#define __VTX_MODEL_REPRESENTATION_LIBRARY__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/base_model.hpp"
#include "representation.hpp"
#include <vector>

namespace VTX
{
	namespace Model
	{
		namespace Representation
		{
			class RepresentationLibrary : public BaseModel
			{
			  public:
				inline static RepresentationLibrary & get()
				{
					static RepresentationLibrary instance;
					return instance;
				};

				BaseRepresentation * const getRepresentation( const int p_index )
				{
					if ( 0 <= p_index && p_index < _representations.size() )
						return _representations[ p_index ];

					return nullptr;
				};
				const BaseRepresentation * const getRepresentation( const int p_index ) const
				{
					if ( 0 <= p_index && p_index < _representations.size() )
						return _representations[ p_index ];

					return nullptr;
				};
				BaseRepresentation * const getRepresentationByName( const std::string & p_name )
				{
					for ( BaseRepresentation * const it : _representations )
					{
						if ( it->getName() == p_name )
							return it;
					}

					return nullptr;
				};
				const BaseRepresentation * const getRepresentationByName( const std::string & p_name ) const
				{
					for ( auto it : _representations )
					{
						if ( it->getName() == p_name )
							return it;
					}

					return nullptr;
				};

				inline int getRepresentationCount() const { return (int)_representations.size(); };

				inline void addRepresentation( BaseRepresentation * const p_representation ) { _representations.emplace_back( p_representation ); };
				inline void removeRepresentation( const int p_index )
				{
					if ( 0 <= p_index && p_index < _representations.size() )
						_representations.erase( _representations.begin() + p_index );
				};

			  private:
				inline RepresentationLibrary() : BaseModel( ID::Model::MODEL_SELECTION_LIBRARY ) { init(); };
				void init();

				std::vector<BaseRepresentation *> _representations;
			};
		} // namespace Representation
	}	  // namespace Model
} // namespace VTX
#endif
