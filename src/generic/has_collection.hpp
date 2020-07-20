#ifndef __VTX_HAS_COLLECTION__
#define __VTX_HAS_COLLECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_cleanable.hpp"
#include "base_collectionable.hpp"
#include "base_initializable.hpp"
#include "factory.hpp"
#include <algorithm>
#include <map>
#include <string>
#include <type_traits>
#include <utility>

namespace VTX
{
	namespace Generic
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseCollectionable, T>::value>>
		class HasCollection : public BaseInitializable, public BaseCleanable
		{
		  public:
			using MapStringToItemPtr  = std::map<std::string, T *>;
			using PairStringToItemPtr = std::pair<const std::string, T *>;

			virtual void init() override { _addItems(); }

			virtual void clean() override
			{
				for ( const PairStringToItemPtr & pair : _items )
				{
					Generic::destroy<BaseCollectionable>( pair.second );
				}
				_items.clear();
				_orderedKeys.clear();
			}

			void addItem( const std::string & p_name, T * const p_item )
			{
				try
				{
					_items.try_emplace( p_name, p_item );
					_orderedKeys.push_back( p_name );
				}
				catch ( const std::exception & )
				{
					_orderedKeys.erase( std::find( _orderedKeys.begin(), _orderedKeys.end(), p_name ) );
					VTX_WARNING( "An item with this name already exists: " + p_name );
				}
			}

			void addItem( T * const p_item )
			{
				std::string name = static_cast<BaseCollectionable *>( p_item )->getName();
				try
				{
					_items.try_emplace( name, p_item );
					_orderedKeys.push_back( name );
				}
				catch ( const std::exception & )
				{
					_orderedKeys.erase( std::find( _orderedKeys.begin(), _orderedKeys.end(), name ) );
					VTX_WARNING( "An item with this name already exists: "
								 + static_cast<BaseCollectionable *>( p_item )->getName() );
				}
			}

			T * removeItem( const std::string & p_name )
			{
				T * item = _items.at( p_name );
				_items.erase( p_name );
				_orderedKeys.erase( std::find( _orderedKeys.begin(), _orderedKeys.end(), p_name ) );
				return item;
			}

			inline const T * const getItemAt( const std::string & p_name ) const { return _items.at( p_name ); }
			inline T * const	   getItemAt( const std::string & p_name ) { return _items.at( p_name ); }
			inline const T * const getItem( const std::string & p_name ) const { return _items[ p_name ]; }
			inline T * const	   getItem( const std::string & p_name ) { return _items[ p_name ]; }

			template<typename T2, typename = std::enable_if<std::is_base_of<T, T2>::value>>
			const T2 * const getItem( const std::string & p_name ) const
			{
				return dynamic_cast<const T2 * const>( getItem( p_name ) );
			}
			template<typename T2, typename = std::enable_if<std::is_base_of<T, T2>::value>>
			T2 * const getItem( const std::string & p_name )
			{
				return dynamic_cast<T2 * const>( getItem( p_name ) );
			}

			inline bool hasItem( const std::string & p_name ) const { return _items.find( p_name ) != _items.end(); }

		  protected:
			inline MapStringToItemPtr &		  _getItems() { return _items; }
			inline std::vector<std::string> & _getOrderedKeys() { return _orderedKeys; }

			virtual void _addItems() {};

		  private:
			MapStringToItemPtr		 _items		  = MapStringToItemPtr();
			std::vector<std::string> _orderedKeys = std::vector<std::string>();
		};
	} // namespace Generic
} // namespace VTX
#endif
