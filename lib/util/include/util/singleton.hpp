#ifndef __VTX_UTIL_SINGLETON__
#define __VTX_UTIL_SINGLETON__

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
