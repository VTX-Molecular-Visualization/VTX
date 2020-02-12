#ifndef __VTX_FACTORIES__
#define __VTX_FACTORIES__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_initializable.hpp"
#include <type_traits>
#include <utility>

namespace VTX
{
	namespace Generic
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseInitializable, T>::value>>
		class FactoryInitializable
		{
		  public:
			static T * const create()
			{
				T * const instance = new T();
				instance->init();
				return instance;
			}
		};
	} // namespace Generic
} // namespace VTX
#endif
