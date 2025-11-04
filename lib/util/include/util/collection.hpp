#ifndef __VTX_UTIL_COLLECTION__
#define __VTX_UTIL_COLLECTION__

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
	 * @brief A utility class to store a collection of objects, mapped on a given hash, or generated hash from typename.
	 * @tparam C is the stored objects type.
	 */
	template<typename C>
	class Collection
	{
	  public:
		template<typename T>
		inline bool has() const
		{
			return has( hash<T>() );
		}

		inline bool has( const Hash & p_hash ) const { return _map.contains( p_hash ); }

		template<typename T>
		inline T * const get() const
		{
			return get<T>( hash<T>() );
		}

		template<typename T>
		inline T * const getOrCreate()
		{
			if ( has<T>() )
			{
				return get<T>( hash<T>() );
			}
			else
			{
				return create<T>();
			}
		}

		template<typename T>
		inline T * const getOrCreateWithHash( const Hash & p_hash )
		{
			if ( has( p_hash ) )
			{
				return get<T>( p_hash );
			}
			else
			{
				return createWithHash<T>( p_hash );
			}
		}

		template<typename T>
		inline T * const get( const Hash & p_hash ) const
		{
			assert( _map.contains( p_hash ) );

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
			/*
			else if constexpr ( std::is_class_v<C> )
			{
				return static_cast<T *>( &_map[ p_hash ] );
			}
			*/
			else
			{
				static_assert( std::is_same_v<T, void>, "Util::Collection::create(): unrecognized type." );
			}
		}

		template<typename T>
		inline T & getRef() const
		{
			return *get<T>();
		}

		template<typename T, typename... Args>
		inline T * const create( Args &&... p_args )
		{
			return _create<T, Args...>( hash<T>(), std::forward<Args>( p_args )... );
		}

		template<typename T, typename... Args>
		inline T * const createWithHash( const Hash & p_hash, Args &&... p_args )
		{
			return _create<T, Args...>( p_hash, std::forward<Args>( p_args )... );
		}

		template<typename T, typename V>
		inline void set( V * const p_value )
		{
			set<T>( hash<T>(), p_value );
		}

		template<typename T>
		inline void set( const Hash & p_hash, T * const p_value )
		{
			if ( _map.contains( p_hash ) )
			{
				remove( p_hash );
			}

			// Only raw pointer.
			if constexpr ( std::is_pointer<C>::value )
			{
				_map[ p_hash ] = static_cast<C>( p_value );
			}
			// Smart pointer.
			else if constexpr ( IsSmartPtr<C> )
			{
				_map[ p_hash ] = std::unique_ptr<T>( p_value );
			}
			/*
			else if constexpr ( std::is_class_v<C> )
			{
				_map[ p_hash ] = static_cast<C>( *p_value );
			}
			*/
			else
			{
				static_assert( std::is_same_v<T, void>, "Util::Collection::create(): unrecognized type." );
			}
		}

		template<typename T>
		inline void remove()
		{
			remove( hash<T>() );
		}

		inline void remove( const Hash & p_hash )
		{
			assert( _map.contains( p_hash ) );
			// Removed if raw ptr (not proprietary), deleted if unique_ptr (proprietary).
			_map.erase( p_hash );
		}

		inline bool empty() const { return _map.empty(); }

		inline void clear() { _map.clear(); }

		template<typename T>
		static constexpr Hash hash()
		{
#if DEBUG_HASH
			VTX_DEBUG( "Hash: {}", Util::typeName<T>() );
#endif
			return Util::hash<T>();
		}

		auto begin() noexcept { return _map.begin(); }
		auto end() noexcept { return _map.end(); }

		auto begin() const noexcept { return _map.begin(); }
		auto end() const noexcept { return _map.end(); }

		auto cbegin() const noexcept { return _map.cbegin(); }
		auto cend() const noexcept { return _map.cend(); }

	  private:
		mutable std::unordered_map<Hash, C> _map;

		template<typename T, typename... Args>
		inline T * const _create( const Hash & p_hash, Args &&... p_args )
		{
			assert( not _map.contains( p_hash ) );

			// Raw pointer.
			if constexpr ( std::is_pointer<C>::value )
			{
				_map.emplace( p_hash, new T( std::forward<Args>( p_args )... ) );
			}
			// Smart pointer.
			else if constexpr ( IsSmartPtr<C> )
			{
				_map.emplace( p_hash, std::make_unique<T>( std::forward<Args>( p_args )... ) );
			}
			/*
			else if constexpr ( std::is_class_v<C> )
			{
				_map.emplace( p_hash, T( std::forward<Args>( p_args )... ) );
			}
			*/
			else
			{
				static_assert( std::is_same_v<T, void>, "Util::Collection::create(): unrecognized type." );
			}

			return get<T>( p_hash );
		}

		template<typename T>
		inline static void _createHelper( Collection * const p_collection )
		{
			p_collection->create<T>();
		}
	};

} // namespace VTX::Util
#endif
