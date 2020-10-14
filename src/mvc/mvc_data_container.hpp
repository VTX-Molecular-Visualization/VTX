#ifndef __VTX_MVC_DATA_CONTAINER__
#define __VTX_MVC_DATA_CONTAINER__

#ifdef _MSC_VER
#pragma once
#endif

#include "id.hpp"
#include "model/base_model.hpp"
#include "mvc_data.hpp"
#include "view/base_view.hpp"
#include <algorithm>
#include <vector>

namespace VTX
{
	namespace MVC
	{
		class MvcDataContainer
		{
			using VecIt		  = std::vector<MvcData>::iterator;
			using VecSizeType = std::vector<MvcData>::size_type;

		  public:
			MvcDataContainer() { _vector = std::vector<MvcData *>(); };

			MvcData * operator[]( const Model::BaseModel * const p_model ) { return _vector[ _findIndex( p_model->getId() ) ]; };

			inline void		 add( MvcData * p_obj ) { _vector.push_back( p_obj ); };
			inline MvcData * remove( const Model::BaseModel *const p_model )
			{
				VecSizeType index = _findIndex( p_model );
				return _removeAtIndex( index );
			}
			inline MvcData * remove( MvcData * p_obj )
			{
				VecSizeType index = _findIndex( p_obj );
				return _removeAtIndex( index );
			};

		  private:
			std::vector<MvcData *> _vector;

			inline MvcData * _removeAtIndex( const VecSizeType p_index )
			{
				MvcData * removed = _vector[ p_index ];
				_vector.erase( _vector.begin() + p_index );

				return removed;
			}

			VecSizeType _findIndex( const MvcData * const p_obj ) const { return _findIndexRecursive( 0, _vector.size() - 1, p_obj->getId() ); };
			VecSizeType _findIndex( const Model::BaseModel * const p_model ) const { return _findIndexRecursive( 0, _vector.size() - 1, p_model->getId() ); };
			VecSizeType _findIndex( const Model::Model_ID p_id ) const { return _findIndexRecursive( 0, _vector.size() - 1, p_id ); };
			VecSizeType _findIndexRecursive( const VecSizeType p_minIndex, const VecSizeType p_maxIndex, const Model::Model_ID p_searchedValue ) const
			{
				if ( p_maxIndex <= p_minIndex )
				{
					if ( _vector[ p_minIndex ]->getId() == p_searchedValue )
						return p_minIndex;
					else
						throw new std::exception( "searchedValue not found !" );
				}
				VecSizeType		middleIndex = p_maxIndex / 2 + p_minIndex / 2;
				Model::Model_ID middleValue = _vector[ middleIndex ]->getId();

				if ( middleValue < p_searchedValue )
					return _findIndexRecursive( middleIndex + 1, p_maxIndex, p_searchedValue );
				else if ( middleValue > p_searchedValue )
					return _findIndexRecursive( p_minIndex, middleIndex - 1, p_searchedValue );
				else // middleValue == searchedValue
					return middleIndex;
			};
		};
	} // namespace MVC
} // namespace VTX

#endif
