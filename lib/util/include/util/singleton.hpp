#ifndef __VTX_UTIL_BASE_STATIC_SINGLETON__
#define __VTX_UTIL_BASE_STATIC_SINGLETON__

#include <initializer_list>
#include <unordered_map>
#include <util/hashing.hpp>

namespace VTX::Util
{
	template<typename T>
	class Singleton final
	{
	  public:
		inline static T & get()
		{
			static T instance;
			return instance;
		}
	};

} // namespace VTX::Util
#endif
