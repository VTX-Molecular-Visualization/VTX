#ifndef __VTX_UTIL_GENERIC_BASE_STATIC_SINGLETON__
#define __VTX_UTIL_GENERIC_BASE_STATIC_SINGLETON__

#include <initializer_list>
#include <unordered_map>
#include <util/hashing.hpp>

namespace VTX::Util::Generic
{

	template<typename T>
	class BaseStaticSingleton
	{
	  public:
		static T & get()
		{
			static T instance { StructPrivacyToken {} };
			return instance;
		}

		BaseStaticSingleton( const BaseStaticSingleton & ) = delete;
		BaseStaticSingleton( std::initializer_list<int> )  = delete;
		BaseStaticSingleton( BaseStaticSingleton && )	   = delete;

		BaseStaticSingleton & operator=( const BaseStaticSingleton ) = delete;
		BaseStaticSingleton & operator=( BaseStaticSingleton && )	 = delete;

	  protected:
		struct StructPrivacyToken
		{
		};
		BaseStaticSingleton() = default;
	};

	template<typename T>
	class UniqueInstance final
	{
	  public:
		static T & get()
		{
			static T instance;
			return instance;
		}
	};

} // namespace VTX::Util::Generic
#endif
