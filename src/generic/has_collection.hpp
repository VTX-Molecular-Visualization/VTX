#ifndef __VTX_HAS_COLLECTION__
#define __VTX_HAS_COLLECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_collectionable.hpp"
#include "util/type.hpp"
#include <map>
#include <string>
#include <type_traits>
#include <utility>

namespace VTX
{
	namespace Generic
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseCollectionable, T>::value>>
		class HasCollection
		{
		  public:
			using MapStringToItemPtr  = std::map<std::string, T *>;
			using PairStringToItemPtr = std::pair<std::string, T *>;

			virtual ~HasCollection() { clear(); }

			virtual void init() { _addItems(); }

			virtual void clear()
			{
				for ( const PairStringToItemPtr pair : _items )
				{
					( (BaseCollectionable *)pair.second )->cleanItem();
					delete pair.second;
				}
				_items.clear();
			}

			virtual T * const findItem( const std::string & p_name )
			{
				if ( _items.find( p_name ) != _items.end() ) { return _items.at( p_name ); }
				else
				{
					return nullptr;
				}
			}
			void addItem( T * const p_item )
			{
				( (BaseCollectionable *)p_item )->initItem();
				try
				{
					_items.try_emplace( ( (BaseCollectionable *)p_item )->getName(), p_item );
				}
				catch ( const std::exception & )
				{
					VTX_WARNING( "An item with this name already exists: "
								 + ( (BaseCollectionable *)p_item )->getName() );
				}
			}

			void addItemRef( T * const p_item )
			{
				try
				{
					_items.try_emplace( ( (BaseCollectionable *)p_item )->getName(), p_item );
				}
				catch ( const std::exception & )
				{
					VTX_WARNING( "An item with this name already exists: "
								 + ( (BaseCollectionable *)p_item )->getName() );
				}
			}

			void removeItem( const std::string & p_name )
			{
				BaseCollectionable * item = (BaseCollectionable *)_items.at( p_name );
				( (BaseCollectionable *)item )->cleanItem();
				delete item;
				_items.erase( p_name );
			}

			void removeItemRef( const std::string & p_name ) { _items.erase( p_name ); }

		  protected:
			inline MapStringToItemPtr & _getItems() { return _items; }
			inline T * const			_getItem( const std::string & p_name ) const { return _items.at( p_name ); }
			inline bool _hasItem( const std::string & p_name ) const { return _items.find( p_name ) != _items.end(); }

			virtual void _addItems() {};

		  private:
			MapStringToItemPtr _items = MapStringToItemPtr();
		};
	} // namespace Generic
} // namespace VTX
#endif
