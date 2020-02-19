#ifndef __VTX_HAS_COLLECTION__
#define __VTX_HAS_COLLECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_cleanable.hpp"
#include "base_collectionable.hpp"
#include "base_initializable.hpp"
#include "factory.hpp"
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
			using PairStringToItemPtr = std::pair<std::string, T *>;

			virtual void init() override { _addItems(); }

			virtual void clean() override
			{
				VTX_DEBUG( "Cleaning collection" );
				for ( const PairStringToItemPtr pair : _items )
				{
					VTX_DEBUG( "Removing item: " + pair.first );
					Generic::destroy<BaseCollectionable>( pair.second );
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
				VTX_DEBUG( "Adding item: " + static_cast<BaseCollectionable *>( p_item )->getName() );
				try
				{
					_items.try_emplace( static_cast<BaseCollectionable *>( p_item )->getName(), p_item );
				}
				catch ( const std::exception & )
				{
					VTX_WARNING( "An item with this name already exists: "
								 + static_cast<BaseCollectionable *>( p_item )->getName() );
				}
			}

			virtual T * removeItem( const std::string & p_name )
			{
				VTX_DEBUG( "Removing item: " + p_name );
				T * item = _items.at( p_name );
				_items.erase( p_name );
				return item;
			}

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
