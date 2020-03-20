#ifndef __VTX_FACTORY__
#define __VTX_FACTORY__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_cleanable.hpp"
#include "base_initializable.hpp"
#include <map>
#include <type_traits>
#include <utility>
#include <vector>

namespace VTX
{
	namespace Generic
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseInitializable, T>::value>>
		static T * const create()
		{
			T * const instance = new T();
			instance->init();
			return instance;
		}

		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseCleanable, T>::value>>
		static void destroy( T * p_instance )
		{
			p_instance->clean();
			delete p_instance;
		}

		template<typename M,
				 typename V,
				 typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>,
				 typename = std::enable_if<std::is_base_of<View::BaseView<M>, V>::value>>
		static V * const create( M * const p_model )
		{
			V * const instance = new V( p_model );
			instance->init();
			return instance;
		}

		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseCleanable, T>::value>>
		void clearVector( std::vector<T *> & p_vector )
		{
			for ( T * element : p_vector )
			{
				destroy( element );
			}
			p_vector.clear();
		}

		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseCleanable, T>::value>>
		void clearStringMap( std::map<std::string, T *> & p_map )
		{
			for ( std::pair<std::string, T *> pair : p_map )
			{
				destroy( pair.second );
			}
			p_map.clear();
		}
	} // namespace Generic
} // namespace VTX
#endif
