#ifndef __VTX_UTIL_OLLECTION__
#define __VTX_UTIL_OLLECTION__

#include <functional>
#include <iostream>
#include <unordered_map>
#include <util/concepts.hpp>
#include <util/hashing.hpp>
#include <util/singleton.hpp>

#define DEBUG_HASH 0

namespace VTX::Util
{
	// using CollectionKey = std::string_view;

	/*
	template<typename C>
	concept ConceptCollectionable = requires( C p_collectionable ) {
		{ p_collectionable.getName() } -> std::same_as<CollectionKey>;
	};
	*/

	/**
	 * @brief A utility class to store a collecion of objects, mapped on a given hash, or generated hash from typename.
	 * @tparam C is the stored objects type.
	 */
	template<typename C>
	class Collection
	{
	  public:
		/**
		 * @brief Auto register the derived class in a static collection.
		 * @tparam T is the derived class type.
		 */
		/*
		template<typename T>
		class GlobalStorage
		{
		  public:
			GlobalStorage() { Singleton<Collection<C>>::get().template set<T>( static_cast<T *>( this ) ); }
			virtual ~GlobalStorage() { Singleton<Collection<C>>::get().template remove<T>(); }
		};
		*/
		/**
		 * @brief Register a key to create item from.
		 * @tparam T is the derived class type.
		 */
		/*
		template<typename T>
		class Registration final
		{
		  public:
			Registration( const CollectionKey & p_key = Util::typeName<T>() )
			{
				Singleton<Collection<C>>::get().template registerKey<T>( p_key );
			}
		};
		*/

	  public:
		/*		template<typename T>
		inline void registerKey( const CollectionKey & p_key )
		{
			assert( not _creators.contains( p_key ) );
			_creators[ p_key ] = &Collection::_createHelper<T>;
		}
		*/

		template<typename T>
		inline bool has() const
		{
			return has( hash<T>() );
		}

		inline bool has( const Hash & p_hash ) const { return _map.contains( p_hash ); }

		template<typename T>
		inline T * const get()
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
		inline T * const get( const Hash & p_hash )
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
			else
			{
				static_assert( std::is_same_v<T, void>, "Util::Collection::get(): unrecognized type." );
				return nullptr;
			}
		}

		template<typename T>
		inline T & getRef()
		{
			return *get<T>();
		}

		/*
		template<typename T>
		inline T * const createFromKey( const CollectionKey & p_key )
		{
			assert( _creators.contains( p_key ) );

			_creators[ p_key ]( this );
			return get<T>();
		}
		*/

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
			else
			{
				static_assert( std::is_same_v<T, void>, "Util::Collection::set(): unrecognized type." );
			}
		}

		template<typename T>
		inline void remove()
		{
			remove( hash<T>() );
		}

		// inline void remove( const CollectionKey & p_key ) { remove( Util::hash( p_key ) ); }

		inline void remove( const Hash & p_hash )
		{
			assert( _map.contains( p_hash ) );
			// Removed if raw ptr (not proprietary), deleted if unique_ptr (proprietary).
			_map.erase( p_hash );
		}

		inline void clear() { _map.clear(); }

		template<typename T>
		static constexpr Hash hash()
		{
#if DEBUG_HASH
			VTX_DEBUG( "Hash: {}", Util::typeName<T>() );
#endif
			return Util::hash<T>();
		}

	  private:
		// using CreateFunc = void ( * )( Collection * const );

		std::unordered_map<Hash, C> _map;
		// std::unordered_map<CollectionKey, CreateFunc> _creators;

		template<typename T, typename... Args>
		inline T * _create( const Hash & p_hash, Args &&... p_args )
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
