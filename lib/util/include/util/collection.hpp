#ifndef __VTX_UTIL_HASHED_COLLECTION__
#define __VTX_UTIL_HASHED_COLLECTION__

#include <functional>
#include <iostream>
#include <unordered_map>
#include <util/concepts.hpp>
#include <util/hashing.hpp>
#include <util/singleton.hpp>

#define DEBUG_HASH 0

namespace VTX::Util
{
	/**
	 * @brief A utility class to store a collecion of objects, mapped on a given hash, or generated hash from typename.
	 * @tparam C is the stored objects type.
	 */
	template<typename C>
	class Collection
	{
	  public:
		using Key = std::string_view;

		/**
		 * @brief Auto register the derived class in a static collection.
		 * @tparam T is the derived class type.
		 */
		/*
		template<typename T>
		class GlobalStorage
		{
		  public:
			GlobalStorage( const C & p_instance ) { Singleton<Collection<C>>::get().template set<T>( p_instance ); }
			virtual ~GlobalStorage() { Singleton<Collection<C>>::get().template remove<T>(); }
		};
		*/

		/**
		 * @brief Register a key to create to create item from.
		 * @tparam T is the derived class type.
		 */
		template<typename T>
		class Registration final
		{
		  public:
			Registration( const Key & p_key = Util::typeName<T>() )
			{
				Singleton<Collection<C>>::get().template registerKey<T>( p_key );
			}
		};

	  public:
		template<typename T>
		inline void registerKey( const Key & p_key )
		{
			_creators[ p_key ] = [ this ]() { create<T>(); };
		}

		template<typename T>
		inline bool has()
		{
			return has( hash<T>() );
		}

		inline bool has( const Hash & p_hash ) { return _map.contains( p_hash ); }

		template<typename T>
		inline T * const get()
		{
			return _get<T>( hash<T>() );
		}

		template<typename T>
		inline T * const get( const Hash & p_hash )
		{
			return _get<T>( p_hash );
		}

		template<typename T>
		inline T * const _get( const Hash & p_hash )
		{
			if ( not _map.contains( p_hash ) )
			{
				return _create<T>( p_hash );
			}

			// Raw pointer.
			if constexpr ( std::is_pointer<C>::value )
			{
				return static_cast<T *>( _map[ p_hash ] );
			}
			// Smart pointer.
			else if constexpr ( IsSmartPtr<C> )
			{
				return static_cast<T *>( _map[ p_hash ].get() );
			}
			//
			else
			{
				// return static_cast<T *>( &_map[ p_hash ] );
				static_assert( true );
				return nullptr;
			}
		}

		template<typename T>
		inline T * const createFromKey( const Key & p_key )
		{
			assert( _creators.contains( p_key ) );

			_creators[ p_key ]();
			return get<T>();
		}

		template<typename T, typename... Args>
		inline T * const create( Args &&... p_args )
		{
			return _create<T, Args...>( hash<T>(), std::forward<Args>( p_args )... );
		}

		template<typename T, typename... Args>
		inline T * const create( const Hash & p_hash, Args &&... p_args )
		{
			return _create<T, Args...>( p_hash, std::forward<Args>( p_args )... );
		}

		template<typename T, typename... Args>
		inline T * _create( const Hash & p_hash, Args &&... p_args )
		{
			if ( _map.contains( p_hash ) )
			{
				return _get<T>( p_hash );
			}

			// Raw pointer.
			if constexpr ( std::is_pointer<C>::value )
			{
				_map[ p_hash ] = new T( std::forward<Args>( p_args )... );
			}
			// Smart pointer.
			else if constexpr ( IsSmartPtr<C> )
			{
				_map[ p_hash ] = std::make_unique<T>( std::forward<Args>( p_args )... );
			}

			//
			else
			{
				//_map[ p_hash ] = C( std::forward<Args>( p_args )... );
				static_assert( true );
			}

			return _get<T>( p_hash );
		}

		template<typename T>
		inline void set( T * const p_value )
		{
			set<T>( hash<T>(), p_value );
		}

		template<typename T>
		inline void set( const Hash & p_hash, T * const p_value )
		{
			assert( not _map.contains( p_hash ) );

			// Only raw pointer.
			if constexpr ( std::is_pointer<C>::value )
			{
				_map[ p_hash ] = static_cast<C>( p_value );
			}
			else
			{
				static_assert( true );
			}
		}

		template<typename T>
		inline void remove()
		{
			remove( hash<T>() );
		}

		inline void remove( const Key & p_key ) { remove( Util::hash( p_key ) ); }

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

	  private:
		using CreateFunc = std::function<void()>;

		std::unordered_map<Hash, C>			_map;
		std::unordered_map<Key, CreateFunc> _creators;
	};

} // namespace VTX::Util
#endif
