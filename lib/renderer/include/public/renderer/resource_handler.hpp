#ifndef __VTX_RENDERER_CONTEXT_HANDLE_MANAGER__
#define __VTX_RENDERER_CONTEXT_HANDLE_MANAGER__

#include "renderer/descriptors.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <new>
#include <unordered_map>
#include <util/logger.hpp>
#include <utility>
#include <vector>

namespace VTX::Renderer
{
	/**
	 * @brief Handle-only resource pool.
	 */
	template<typename T>
	class ResourcePool
	{
	  private:
		/**
		 * @brief Slot to store destructible and aligned resource.
		 */
		struct _Slot
		{
			_Slot() = default;

			_Slot( const _Slot & )			   = delete;
			_Slot & operator=( const _Slot & ) = delete;

			_Slot( _Slot && p_other )
			{
				if ( p_other.alive )
				{
					construct( std::move( p_other.get() ) );
				}
			}

			~_Slot() { destroy(); }

			template<typename... Args>
			T & construct( Args &&... p_args )
			{
				destroy();
				T * data = std::construct_at( _data(), std::forward<Args>( p_args )... );
				alive	 = true;

				return *data;
			}

			void destroy() noexcept
			{
				if ( alive )
				{
					std::destroy_at( _data() );
					alive = false;
				}
			}

			T & get() noexcept
			{
				assert( alive );
				return *_data();
			}

			const T & get() const noexcept
			{
				assert( alive );
				return *_data();
			}

			bool alive = false;

		  private:
			T * _data() noexcept { return std::launder( reinterpret_cast<T *>( _storage ) ); }

			const T * _data() const noexcept { return std::launder( reinterpret_cast<const T *>( _storage ) ); }

			alignas( T ) std::byte _storage[ sizeof( T ) ];
		};

	  public:
		/**
		 * @brief Emplace a new resource.
		 * Reuse available handles if any.
		 */
		template<typename... Args>
		Desc::Handle emplace( Args &&... p_args )
		{
			Desc::Handle handle;
			if ( not _availables.empty() )
			{
				handle = _availables.back();
				_availables.pop_back();
			}
			else
			{
				handle = static_cast<Desc::Handle>( _resources.size() );
				_resources.emplace_back();
			}

			_construct( handle, std::forward<Args>( p_args )... );
			_validateStorage( handle );

			return handle;
		}

		/**
		 * @brief Erase a resource from handle.
		 */
		void erase( const Desc::Handle p_handle )
		{
			if ( not contains( p_handle ) )
			{
				return;
			}

			_resources[ p_handle ].destroy();
			_availables.push_back( p_handle );
		}

		/**
		 * @brief Clear all resources.
		 */
		void clear()
		{
			_resources.clear();
			_availables.clear();
			_invalids.clear();
		}

		inline bool contains( const Desc::Handle p_handle ) const noexcept
		{
			return p_handle < _resources.size() && _resources[ p_handle ].alive;
		}

		inline bool validate( const Desc::Handle p_handle ) noexcept
		{
			std::erase( _invalids, p_handle );

			return contains( p_handle );
		}

		/**
		 * @brief Get the number of valid resources.
		 */
		inline size_t size() const { return _resources.size() - _availables.size(); }

		inline bool empty() const { return size() == 0; }

		/**
		 * @brief Access resource by handle.
		 */
		inline const T & get( const Desc::Handle p_handle ) const noexcept
		{
			assert( p_handle < _resources.size() );
			assert( std::find( _availables.begin(), _availables.end(), p_handle ) == _availables.end() );
			assert( _resources[ p_handle ].alive );

			return _resources[ p_handle ].get();
		}

		inline T & get( const Desc::Handle p_handle ) noexcept
		{
			assert( p_handle < _resources.size() );
			assert( std::find( _availables.begin(), _availables.end(), p_handle ) == _availables.end() );
			assert( _resources[ p_handle ].alive );

			return _resources[ p_handle ].get();
		}

		/**
		 * @brief Invalidate all resources.
		 */
		void invalidate()
		{
			_invalids.clear();
			for ( Desc::Handle handle = 0; handle < static_cast<Desc::Handle>( _resources.size() ); ++handle )
			{
				if ( contains( handle ) )
				{
					_invalids.push_back( handle );
				}
			}
		}

		/**
		 * @brief Purge invalidated resources.
		 */
		void purge()
		{
			for ( const Desc::Handle handle : _invalids )
			{
				erase( handle );
				VTX_TRACE( "Purging resource handle {}", handle );
			}
			_invalids.clear();
		}

		/**
		 * @brief Debug string.
		 */
		std::string toString() const
		{
			std::string result = "ResourcePool:\n";
			result += "  Total resources: " + std::to_string( _resources.size() ) + "\n";
			result += "  Available handles: " + std::to_string( _availables.size() ) + "\n";
			result += "  Invalid handles: " + std::to_string( _invalids.size() ) + "\n";
			return result;
		}

		/**
		 * @brief Iterator over valid resources.
		 */
		class iterator
		{
		  public:
			using VecIter = typename std::vector<_Slot>::iterator;

			iterator( ResourcePool * p_pool, VecIter p_it ) : _pool( p_pool ), _it( p_it ) { skipInvalid(); }

			iterator & operator++()
			{
				++_it;
				skipInvalid();
				return *this;
			}

			bool operator!=( const iterator & p_other ) const { return _it != p_other._it; }

			T & operator*() const { return _it->get(); }

		  private:
			void skipInvalid()
			{
				while ( _it != _pool->_resources.end() && not _it->alive )
				{
					++_it;
				}
			}

			ResourcePool * _pool;
			VecIter		   _it;
		};

		class const_iterator
		{
		  public:
			using VecIter = typename std::vector<_Slot>::const_iterator;

			const_iterator( const ResourcePool * p_pool, VecIter p_it ) : _pool( p_pool ), _it( p_it )
			{
				skipInvalid();
			}

			const_iterator & operator++()
			{
				++_it;
				skipInvalid();
				return *this;
			}

			bool operator!=( const const_iterator & p_other ) const { return _it != p_other._it; }

			const T & operator*() const { return _it->get(); }

		  private:
			void skipInvalid()
			{
				while ( _it != _pool->_resources.end() && not _it->alive )
				{
					++_it;
				}
			}

			const ResourcePool * _pool;
			VecIter				 _it;
		};

		iterator begin() { return iterator( this, _resources.begin() ); }

		iterator end() { return iterator( this, _resources.end() ); }

		const_iterator begin() const { return const_iterator( this, _resources.begin() ); }

		const_iterator end() const { return const_iterator( this, _resources.end() ); }

		struct EntryRef
		{
			Desc::Handle handle;
			T &			 resource;
		};

		struct ConstEntryRef
		{
			Desc::Handle handle;
			const T &	 resource;
		};

		class entry_iterator
		{
		  public:
			entry_iterator( ResourcePool * p_pool, const Desc::Handle p_handle ) : _pool( p_pool ), _handle( p_handle )
			{
				skipInvalid();
			}

			entry_iterator & operator++()
			{
				++_handle;
				skipInvalid();
				return *this;
			}

			bool operator!=( const entry_iterator & p_other ) const { return _handle != p_other._handle; }

			EntryRef operator*() const { return EntryRef { _handle, _pool->get( _handle ) }; }

		  private:
			void skipInvalid()
			{
				while ( _handle < _pool->_resources.size() && not _pool->contains( _handle ) )
				{
					++_handle;
				}
			}

			ResourcePool * _pool;
			Desc::Handle   _handle;
		};

		class const_entry_iterator
		{
		  public:
			const_entry_iterator( const ResourcePool * p_pool, const Desc::Handle p_handle ) :
				_pool( p_pool ), _handle( p_handle )
			{
				skipInvalid();
			}

			const_entry_iterator & operator++()
			{
				++_handle;
				skipInvalid();
				return *this;
			}

			bool operator!=( const const_entry_iterator & p_other ) const { return _handle != p_other._handle; }

			ConstEntryRef operator*() const { return ConstEntryRef { _handle, _pool->get( _handle ) }; }

		  private:
			void skipInvalid()
			{
				while ( _handle < _pool->_resources.size() && not _pool->contains( _handle ) )
				{
					++_handle;
				}
			}

			const ResourcePool * _pool;
			Desc::Handle		 _handle;
		};

		struct Entries
		{
			ResourcePool * pool;

			entry_iterator begin() { return entry_iterator( pool, 0 ); }

			entry_iterator end()
			{
				return entry_iterator( pool, static_cast<Desc::Handle>( pool->_resources.size() ) );
			}
		};

		struct ConstEntries
		{
			const ResourcePool * pool;

			const_entry_iterator begin() const { return const_entry_iterator( pool, 0 ); }

			const_entry_iterator end() const
			{
				return const_entry_iterator( pool, static_cast<Desc::Handle>( pool->_resources.size() ) );
			}
		};

		Entries entries() { return Entries { this }; }

		ConstEntries entries() const { return ConstEntries { this }; }

	  protected:
		template<typename... Args>
		T & _construct( const Desc::Handle p_handle, Args &&... p_args )
		{
			assert( p_handle < _resources.size() );
			return _resources[ p_handle ].construct( std::forward<Args>( p_args )... );
		}

		void _validateStorage( const Desc::Handle p_handle ) { std::erase( _invalids, p_handle ); }

	  private:
		std::vector<_Slot>		  _resources;
		std::vector<Desc::Handle> _availables;
		std::vector<Desc::Handle> _invalids;
	};

	/**
	 * @brief Generic resource handler.
	 * Store resources and provide access through handles/keys.
	 */
	template<typename T, typename K = Desc::Key, typename D = void>
	class ResourceHandler : public ResourcePool<T>
	{
	  private:
		using Pool = ResourcePool<T>;

	  public:
		using Pool::begin;
		using Pool::contains;
		using Pool::empty;
		using Pool::end;
		using Pool::entries;
		using Pool::get;
		using Pool::size;

		/**
		 * @brief Cache : mapping Key -> { Handle, Desc }.
		 */
		template<typename _D>
		struct EntryT
		{
			Desc::Handle handle;
			_D			 descriptor;
		};

		template<>
		struct EntryT<void>
		{
			Desc::Handle handle;
		};

		using Entry = EntryT<D>;

		/**
		 * @brief Emplace a new resource.
		 * Reuse available handles if any.
		 */
		template<typename... Args>
		Desc::Handle emplace( const K & p_key, Args &&... p_args )
			requires std::is_same_v<D, void>
		{
			Desc::Handle handle;
			if ( _cache.contains( p_key ) )
			{
				handle = _cache[ p_key ].handle;
				Pool::_construct( handle, std::forward<Args>( p_args )... );
				Pool::_validateStorage( handle );
			}
			else
			{
				handle = Pool::emplace( std::forward<Args>( p_args )... );
			}

			_cache.insert_or_assign( p_key, Entry { handle } );

			return handle;
		}

		/**
		 * @brief Emplace a new resource.
		 * Reuse available handles if any.
		 */
		template<typename _D = D, typename... Args>
		Desc::Handle emplace( const K & p_key, const _D & p_desc, Args &&... p_args )
			requires not std::is_same_v<_D, void>
		{
			Desc::Handle handle;
			if ( _cache.contains( p_key ) )
			{
				handle = _cache[ p_key ].handle;
				Pool::_construct( handle, std::forward<Args>( p_args )... );
				Pool::_validateStorage( handle );
			}
			else
			{
				handle = Pool::emplace( std::forward<Args>( p_args )... );
			}

			_cache.insert_or_assign( p_key, Entry { handle, p_desc } );

			return handle;
		}

		/**
		 * @brief Erase a resource from key.
		 */
		void erase( const K & p_key )
		{
			if ( not _cache.contains( p_key ) )
			{
				return;
			}

			erase( handle( p_key ) );
		}

		/**
		 * @brief Erase a resource from handle.
		 */
		void erase( const Desc::Handle p_handle )
		{
			if ( not Pool::contains( p_handle ) )
			{
				return;
			}

			auto it = std::find_if(
				_cache.begin(), _cache.end(), [ & ]( const auto & pair ) { return pair.second.handle == p_handle; }
			);
			if ( it != _cache.end() )
			{
				_cache.erase( it );
			}

			Pool::erase( p_handle );
		}

		/**
		 * @brief Clear all resources.
		 */
		void clear()
		{
			Pool::clear();
			_cache.clear();
		}

		/**
		 * @brief Check if a resource exists from key.
		 */
		inline bool contains( const K & p_key ) const
		{
			if ( not _cache.contains( p_key ) )
			{
				return false;
			}

			return Pool::contains( handle( p_key ) );
		}

		inline bool contains( const Desc::Handle p_handle ) const noexcept { return Pool::contains( p_handle ); }

		/**
		 * @brief Check if a resource exists from key and remove from invalids if present.
		 */
		template<typename _D = D>
		inline bool validate( const K p_key, const _D p_desc )
			requires not std::is_same_v<_D, void>
		{
			if ( not _cache.contains( p_key ) )
			{
				return false;
			}

			if ( Desc::hashDesc( p_desc ) != Desc::hashDesc( descriptor( p_key ) ) )
			{
				return false;
			}

			return Pool::validate( handle( p_key ) );
		}

		inline bool validate( const K p_key )
			requires std::is_same_v<D, void>
		{
			if ( not _cache.contains( p_key ) )
			{
				return false;
			}

			return Pool::validate( handle( p_key ) );
		}

		inline bool validate( const Desc::Handle p_handle ) noexcept { return Pool::validate( p_handle ); }

		/**
		 * @brief Access handle by key.
		 */
		inline Desc::Handle handle( const K & p_key ) const
		{
			const auto it = _cache.find( p_key );
			assert( it != _cache.end() );
			return it->second.handle;
		}

		/**
		 * @brief Access descriptor by key.
		 */
		template<typename _D = D>
		inline const _D & descriptor( const K & p_key ) const
			requires not std::is_same_v<_D, void>
		{
			const auto it = _cache.find( p_key );
			assert( it != _cache.end() );
			return it->second.descriptor;
		}

		/**
		 * @brief Access resource by key.
		 */
		inline const T & get( const K & p_key ) const { return Pool::get( handle( p_key ) ); }

		inline T & get( const K & p_key ) { return Pool::get( handle( p_key ) ); }

		/**
		 * @brief Get all keys.
		 */
		std::vector<K> keys() const
		{
			std::vector<K> result;
			result.reserve( _cache.size() );
			for ( const auto & [ key, entry ] : _cache )
			{
				result.push_back( key );
			}
			return result;
		}

		/**
		 * @brief Get all handles.
		 */
		std::vector<Desc::Handle> handles() const
		{
			std::vector<Desc::Handle> result;
			result.reserve( _cache.size() );
			for ( const auto & [ key, entry ] : _cache )
			{
				result.push_back( entry.handle );
			}
			return result;
		}

		/**
		 * @brief Invalidate all resources.
		 */
		void invalidate() { Pool::invalidate(); }

		/**
		 * @brief Purge invalidated resources.
		 */
		void purge()
		{
			Pool::purge();
			for ( auto it = _cache.begin(); it != _cache.end(); )
			{
				if ( not Pool::contains( it->second.handle ) )
				{
					it = _cache.erase( it );
				}
				else
				{
					++it;
				}
			}
		}

		/**
		 * @brief Debug string.
		 */
		std::string toString() const
		{
			std::string result = "ResourceHandler:\n";
			result += "  Key count: " + std::to_string( _cache.size() ) + "\n";
			result += Pool::toString();
			return result;
		}

	  private:
		std::unordered_map<K, Entry> _cache;
	};

} // namespace VTX::Renderer

#endif
