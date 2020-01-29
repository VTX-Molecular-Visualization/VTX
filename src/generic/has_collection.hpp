#ifndef __VTX_HAS_COLLECTION__
#define __VTX_HAS_COLLECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_collectionable.hpp"
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

			virtual ~HasCollection() { _items.clear(); }
			virtual void init() { _addItems(); }

		  protected:
			MapStringToItemPtr & _getItems() const { return _items; }
			virtual void		 _addItems() {}
			virtual void		 _addItem( T * const p_item )
			{
				p_item->init();
				try
				{
					_items.try_emplace( p_item->getName(), p_item );
				}
				catch ( const std::exception )
				{
					VTX_WARNING( "An item with this name already exists: " + p_item->getName() );
				}
			}
			virtual void _removeItem( const std::string & p_name ) { _items.erase( p_name ); }

		  private:
			MapStringToItemPtr _items = MapStringToItemPtr();
		};
	} // namespace Generic
} // namespace VTX
#endif
