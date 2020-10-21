#ifndef __VTX_HAS_COLLECTION__
#define __VTX_HAS_COLLECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "factory.hpp"
//#include "vtx_app.hpp"
#include <algorithm>
#include <map>
#include <string>
#include <type_traits>
#include <utility>

namespace VTX
{
	namespace Generic
	{
		template<typename T>
		class HasCollection
		{
		  public:
			using MapStringToItemPtr  = std::map<std::string, T *>;
			using PairStringToItemPtr = std::pair<const std::string, T *>;

			~HasCollection() { clear(); }

			void clear()
			{
				for ( PairStringToItemPtr & pair : _items )
				{
					delete pair.second;
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
					// VTX_WARNING( "An item with this name already exists: " + p_name );
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

			inline const MapStringToItemPtr * const getItems() const { return &_items; }

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
			MapStringToItemPtr		 _items		  = MapStringToItemPtr();
			std::vector<std::string> _orderedKeys = std::vector<std::string>();
		};
	} // namespace Generic
} // namespace VTX
#endif
