#ifndef __VTX_FACTORY__
#define __VTX_FACTORY__

#ifdef _MSC_VER
#pragma once
#endif

#include "view/base_view.hpp"
#include <map>
#include <type_traits>
#include <utility>
#include <vector>

namespace VTX
{
	namespace Generic
	{
		template<typename T>
		void clearVector( std::vector<T *> & p_vector )
		{
			for ( T * element : p_vector )
			{
				delete element;
			}
			p_vector.clear();
		}

		template<typename T, typename V>
		void clearMapAsKey( std::map<T *, V> & p_map )
		{
			for ( std::pair<T *, V> pair : p_map )
			{
				delete pair.first;
			}
			p_map.clear();
		}

		template<typename T, typename V>
		void clearMapAsValue( std::map<V, T *> & p_map )
		{
			for ( std::pair<const V, T *> & pair : p_map )
			{
				delete pair.second;
			}
			p_map.clear();
		}

	} // namespace Generic
} // namespace VTX
#endif
