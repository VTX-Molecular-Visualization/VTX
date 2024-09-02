#ifndef __VTX_UTIL_HASHED_COLLECTION__
#define __VTX_UTIL_HASHED_COLLECTION__

#include <iostream>
#include <unordered_map>
#include <util/hashing.hpp>

#define DEBUG_HASH 0

namespace VTX::Util
{
	/**
	 * @brief A utility class to store a collecion of objects, mapped on a given hash, or generated hash from typename.
	 * @tparam C is the stored objects type.
	 */
	template<typename C>
	class HashedCollection
	{
	  public:
		/**
		 * @brief Auto register the derived class in a static collection.
		 * @tparam T is the derived class type.
		 */
		template<typename T>
		class GlobalStorage
		{
		  public:
			GlobalStorage( const C & p_instance )
			{
				Singleton<HashedCollection<C>>::get().template set<T>( p_instance );
			}
			virtual ~GlobalStorage() { Singleton<HashedCollection<C>>::get().template remove<T>(); }
		};

	  public:
		template<typename T>
		inline bool has()
		{
			return has( hash<T>() );
		}

		inline bool has( const Hash & p_hash ) { return _map.contains( p_hash ); }

		template<typename T>
		inline T get()
		{
			return static_cast<T>( get( hash<T>() ) );
		}

		template<typename T>
		inline T get( const Hash & p_hash )
		{
			return static_cast<T>( get( p_hash ) );
		}

		inline C get( const Hash & p_hash )
		{
			assert( _map.contains( p_hash ) );
			return _map[ p_hash ];
		}

		// TODO: add getOrCreate() with tests.
		template<typename T>
		inline T create()
		{
			return static_cast<T>( create( hash<T>() ) );
		}

		template<typename T>
		inline T create( const Hash & p_hash )
		{
			return static_cast<T>( create( p_hash ) );
		}

		inline C create( const Hash & p_hash )
		{
			assert( not _map.contains( p_hash ) );

			// Raw pointer.
			if constexpr ( std::is_pointer<C>::value )
			{
				_map[ p_hash ] = new std::remove_pointer_t<C>();
			}
			// Smart pointer.
			else if constexpr ( HasPointerOperator<C> )
			{
				_map[ p_hash ] = std::make_unique<std::pointer_traits<C>::element_type>();
			}
			//
			else
			{
				_map[ p_hash ] = C();
			}

			return _map[ p_hash ];
		}

		template<typename T>
		inline void set( const C & p_value )
		{
			set( hash<T>(), p_value );
		}

		inline void set( const Hash & p_hash, const C & p_value )
		{
			assert( not _map.contains( p_hash ) );
			_map[ p_hash ] = p_value;
		}

		template<typename T>
		inline void remove()
		{
			remove( hash<T>() );
		}

		inline void remove( const Hash & p_hash )
		{
			assert( _map.contains( p_hash ) );
			_map.erase( p_hash );
		}

		template<typename T>
		static constexpr Hash hash()
		{
#if DEBUG_HASH == 1
			VTX_DEBUG( "Hash: {}", Util::typeName<T>() );
#endif
			return Util::hash<T>();
		}

		// TODO: replace by custom iterator.
		inline std::unordered_map<Hash, C> & map() { return _map; }

	  private:
		std::unordered_map<Hash, C> _map;
	};

} // namespace VTX::Util
#endif
