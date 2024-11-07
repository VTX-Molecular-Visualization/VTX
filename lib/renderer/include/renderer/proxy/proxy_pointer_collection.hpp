#ifndef __VTX_RENDERER_PROXY_POINTER_COLLECTION__
#define __VTX_RENDERER_PROXY_POINTER_COLLECTION__

#include <util/callback.hpp>
#include <util/types.hpp>

namespace VTX::Renderer::Proxy
{
	class ProxyPointerCollection
	{
	  public:
		template<typename T>
		void set( const uint p_key, const T & p_value )
		{
			settings.emplace( p_key, &p_value );
		}

		template<typename T>
		const T & get( const uint p_key )
		{
			assert( settings.contains( p_key ) );
			assert( settings[ p_key ] != nullptr );

			return *static_cast<const T *>( settings[ p_key ] );
		}

		template<uint, typename... Args>
		Util::Callback<Args...> & onChange()
		{
			static Util::Callback<Args...> callback;
			return callback;
		}

	  private:
		std::unordered_map<uint, const void *> settings;
	};

} // namespace VTX::Renderer::Proxy

#endif
