#ifndef __VTX_FACTORIES__
#define __VTX_FACTORIES__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_cleanable.hpp"
#include "base_initializable.hpp"
#include <type_traits>
#include <utility>

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
	}; // namespace Generic
} // namespace VTX
#endif
