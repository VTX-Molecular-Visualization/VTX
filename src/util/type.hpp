#ifndef __VTX__TYPE__
#define __VTX__TYPE__

#ifdef _MSC_VER
#pragma once
#endif

#include <map>
#include <string>
#include <vector>

namespace VTX
{
	namespace Util
	{
		namespace Type
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

			template<typename T>
			void clearStringMap( std::map<std::string, T *> & p_map )
			{
				for ( std::pair<std::string, T *> pair : p_map )
				{
					delete pair.second;
				}
				p_map.clear();
			}

		} // namespace Type
	}	  // namespace Util
} // namespace VTX

#endif
