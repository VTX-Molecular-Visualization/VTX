#ifndef __VTX_UTIL_HASHED_COLLECTION__
#define __VTX_UTIL_HASHED_COLLECTION__

#include <unordered_map>
#include <util/hashing.hpp>

namespace VTX::Util
{

	template<typename C>
	class HashedCollection
	{
	  public:
		template<typename T>
		inline T get()
		{
			return static_cast<T>( get( _hash<T>() ) );
		}

		inline C get( const Util::Hashing::Hash & p_hash )
		{
			assert( _map.contains( p_hash ) );
			return _map[ p_hash ];
		}

		template<typename T>
		inline void set( const C & p_c )
		{
			set( _hash<T>(), p_c );
		}

		inline void set( const Util::Hashing::Hash & p_hash, const C & p_c )
		{
			assert( not _map.contains( p_hash ) );
			_map[ p_hash ] = p_c;
		}

		template<typename T>
		inline T create()
		{
			auto hash = _hash<T>();
			assert( not _map.contains( hash ) );
			_map[ hash ] = C();
			return _map[ hash ];
		}

		template<typename T>
		inline void remove()
		{
			remove( _hash<T>() );
		}

		inline void remove( const Util::Hashing::Hash & p_hash )
		{
			assert( _map.contains( p_hash ) );
			_map.erase( p_hash );
		}

	  private:
		std::unordered_map<Util::Hashing::Hash, C> _map;

		template<typename T>
		inline Util::Hashing::Hash _hash()
		{
			return Util::Hashing::hash( typeid( T ).name() );
		}
	};

} // namespace VTX::Util
#endif
