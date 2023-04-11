#ifndef __VTX_UTIL_GENERIC_BASE_STATIC_SINGLETON__
#define __VTX_UTIL_GENERIC_BASE_STATIC_SINGLETON__

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

		BaseStaticSingleton( const BaseStaticSingleton & )			 = delete;
		BaseStaticSingleton & operator=( const BaseStaticSingleton ) = delete;
		BaseStaticSingleton( BaseStaticSingleton && )				 = delete;
		BaseStaticSingleton & operator=( BaseStaticSingleton && )	 = delete;

	  protected:
		struct StructPrivacyToken
		{
		};
		BaseStaticSingleton() = default;
	};

} // namespace VTX::Util::Generic
#endif
