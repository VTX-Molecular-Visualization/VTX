#ifndef __VTX_RENDERER_PROXY_POINTER_COLLECTION__
#define __VTX_RENDERER_PROXY_POINTER_COLLECTION__

#include <unordered_map>
#include <util/types.hpp>

namespace VTX::Renderer::Proxy
{
	class ProxyPointerCollection
	{
	  public:
		template<typename T>
		void set( const uint p_key, const T & p_value )
		{
			_collection.emplace( p_key, &p_value );
			_types.emplace( p_key, typeid( T ).hash_code() );
		}

		template<typename T>
		const T & get( const uint p_key )
		{
			assert( _collection.contains( p_key ) );
			assert( _collection[ p_key ] != nullptr );
			assert( _types[ p_key ] == typeid( T ).hash_code() );

			return *static_cast<const T *>( _collection[ p_key ] );
		}

	  private:
		std::unordered_map<uint, const void *> _collection;
		std::unordered_map<uint, const size_t> _types;
	};

} // namespace VTX::Renderer::Proxy

#endif
