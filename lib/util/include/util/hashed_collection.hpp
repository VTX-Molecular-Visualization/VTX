#ifndef __VTX_UTIL_HASHED_COLLECTION__
#define __VTX_UTIL_HASHED_COLLECTION__

#include <iostream>
#include <unordered_map>
#include <util/hashing.hpp>

namespace VTX::Util
{

	template<typename C>
	class HashedCollection
	{
	  public:
		template<typename T>
		inline bool has()
		{
			return has( hash<T>() );
		}

		inline bool has( const Util::Hashing::Hash & p_hash ) { return _map.contains( p_hash ); }

		template<typename T>
		inline T get()
		{
			return static_cast<T>( get( hash<T>() ) );
		}

		inline C get( const Util::Hashing::Hash & p_hash )
		{
			assert( _map.contains( p_hash ) );
			return _map[ p_hash ];
		}

		template<typename T>
		inline void set( const C & p_value )
		{
			set( hash<T>(), p_value );
		}

		inline void set( const Util::Hashing::Hash & p_hash, const C & p_value )
		{
			assert( not _map.contains( p_hash ) );
			_map[ p_hash ] = p_value;
		}

		template<typename T>
		inline void remove()
		{
			remove( hash<T>() );
		}

		inline void remove( const Util::Hashing::Hash & p_hash )
		{
			assert( _map.contains( p_hash ) );
			_map.erase( p_hash );
		}

		template<typename T>
		inline static Util::Hashing::Hash hash()
		{
			// std::cout << typeid( T ).name() << std::endl;
			auto hash = Util::Hashing::hash( typeid( T ).name() );
			// std::cout << hash << std::endl;
			return hash;
		}

	  private:
		std::unordered_map<Util::Hashing::Hash, C> _map;
	};

} // namespace VTX::Util
#endif
