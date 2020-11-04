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
#include <type_traits>
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

			MvcData * const operator[]( const Model::BaseModel * const p_model ) const { return _vector[ _findIndex( p_model->getId() ) ]; };
			MvcData * const operator[]( const Model::ID & p_modelId ) const { return _vector[ _findIndex( p_modelId ) ]; };

			inline void		 add( MvcData * p_obj ) { _vector.push_back( p_obj ); };
			inline MvcData * remove( const Model::BaseModel * const p_model )
			{
				VecSizeType index = _findIndex( p_model );
				return _removeAtIndex( index );
			}
			inline MvcData * remove( const MvcData * const p_obj )
			{
				VecSizeType index = _findIndex( p_obj );
				return _removeAtIndex( index );
			};

			template<typename M, typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>>
			inline void deleteAllMVCs( const std::vector<M *> & p_allModelsSorted )
			{
				VecSizeType		  allModelsSeek	 = 0;
				const VecSizeType allModelsSize	 = p_allModelsSorted.size();
				Model::ID	  currentModelID = p_allModelsSorted[ allModelsSeek ]->getId();

				VecSizeType		  vectorSeek = this->_findIndex( currentModelID );
				VecSizeType		  currIndex	 = vectorSeek;
				const VecSizeType nbItems	 = _vector.size();

				while ( allModelsSeek < allModelsSize )
				{
					if ( _vector[ vectorSeek ]->getId() != currentModelID )
					{
						MvcData * item		  = _vector[ vectorSeek ];
						_vector[ vectorSeek ] = _vector[ currIndex ];
						_vector[ currIndex ]  = item;

						currIndex++;
					}
					else
					{
						allModelsSeek++;

						if ( allModelsSeek < allModelsSize )
							currentModelID = p_allModelsSorted[ allModelsSeek ]->getId();
					}

					vectorSeek++;
				}

				// We finish to decale all the models to pop_back all MVCs
				while ( vectorSeek < nbItems )
				{
					MvcData * item		  = _vector[ vectorSeek ];
					_vector[ vectorSeek ] = _vector[ currIndex ];
					_vector[ currIndex ]  = item;

					vectorSeek++;
					currIndex++;
				}

				while ( currIndex < vectorSeek )
				{
					delete _vector.back();
					_vector.pop_back();
					vectorSeek--;
				}
			}

		  private:
			std::vector<MvcData *> _vector;

			inline MvcData * _removeAtIndex( const VecSizeType & p_index )
			{
				MvcData * const removed = _vector[ p_index ];
				_vector.erase( _vector.begin() + p_index );

				return removed;
			}

			VecSizeType _findIndex( const MvcData * const p_obj ) const { return _findIndexRecursive( 0, _vector.size() - 1, p_obj->getId() ); };
			VecSizeType _findIndex( const Model::BaseModel * const p_model ) const { return _findIndexRecursive( 0, _vector.size() - 1, p_model->getId() ); };
			VecSizeType _findIndex( const Model::ID & p_id ) const { return _findIndexRecursive( 0, _vector.size() - 1, p_id ); };
			VecSizeType _findIndexRecursive( const VecSizeType & p_minIndex, const VecSizeType & p_maxIndex, const Model::ID & p_searchedValue ) const
			{
				if ( p_maxIndex <= p_minIndex )
				{
					if ( _vector[ p_minIndex ]->getId() == p_searchedValue )
						return p_minIndex;
					else
						throw new std::exception( "searchedValue not found !" );
				}
				const VecSizeType	  middleIndex = p_maxIndex / 2 + p_minIndex / 2;
				const Model::ID middleValue = _vector[ middleIndex ]->getId();

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
