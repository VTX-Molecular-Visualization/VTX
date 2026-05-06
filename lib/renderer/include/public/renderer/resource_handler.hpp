#ifndef __VTX_RENDERER_CONTEXT_HANDLE_MANAGER__
#define __VTX_RENDERER_CONTEXT_HANDLE_MANAGER__

#include "renderer/descriptors.hpp"
#include <algorithm>
#include <util/logger.hpp>
#include <vector>

namespace VTX::Renderer
{
	/**
	 * @brief Empty descriptor.
	 */
	struct DescDummy
	{
	};

	/**
	 * @brief Generic resource handler.
	 * Store resources and provide access through handles.
	 */
	template<typename T, typename D = DescDummy, typename K = Desc::Key>
	class ResourceHandler
	{
	  public:
		/**
		 * @brief Cache : mapping Key -> { Handle, Desc }.
		 */
		struct Entry
		{
			Desc::Handle handle;
			D			 descriptor;
		};

		/**
		 * @brief Emplace a new resource.
		 * Reuse available handles if any.
		 */
		template<typename... Args>
		Desc::Handle emplace( const K & p_key, const D & p_desc = {}, Args &&... p_args )
		{
			Desc::Handle handle;
			// Existing resource, update it.
			if ( _cache.contains( p_key ) )
			{
				handle				 = _cache.at( p_key ).handle;
				_resources[ handle ] = std::make_unique<T>( std::forward<Args>( p_args )... );
			}
			// Reuse available handle if any.
			else if ( not _availables.empty() )
			{
				handle = _availables.back();
				_availables.pop_back();
				_resources[ handle ] = std::make_unique<T>( std::forward<Args>( p_args )... );
			}
			// Create new handle.
			else
			{
				handle = static_cast<Desc::Handle>( _resources.size() );
				_resources.emplace_back( std::make_unique<T>( std::forward<Args>( p_args )... ) );
			}

			// Update cache.
			_cache.insert_or_assign( p_key, Entry { handle, p_desc } );

			// Remove from invalids if present.
			auto it = std::find( _invalids.begin(), _invalids.end(), handle );
			if ( it != _invalids.end() )
			{
				_invalids.erase( it );
			}

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
			if ( p_handle >= _resources.size() )
			{
				return;
			}
			if ( _resources[ p_handle ] == nullptr )
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

			_resources[ p_handle ].reset();
			_availables.push_back( p_handle );
		}

		/**
		 * @brief Clear all resources.
		 */
		void clear()
		{
			_resources.clear();
			_cache.clear();
			_availables.clear();
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

			return contains( handle( p_key ) );
		}

		inline bool contains( const Desc::Handle p_handle ) const noexcept
		{ return p_handle < _resources.size() && _resources[ p_handle ]; }

		/**
		 * @brief Check if a resource exists from key and remove from invalids if present.
		 */
		inline bool validate( const K p_key, const D p_desc = {} )
		{
			if ( not _cache.contains( p_key ) )
			{
				return false;
			}

			if ( Desc::hashDesc( p_desc ) != Desc::hashDesc( descriptor( p_key ) ) )
			{
				return false;
			}

			const Desc::Handle h = handle( p_key );

			return validate( h );
		}

		inline bool validate( const Desc::Handle p_handle ) noexcept
		{
			std::erase( _invalids, p_handle );

			return contains( p_handle );
		}

		/**
		 * @brief Access handle by key.
		 */
		inline Desc::Handle handle( const K & p_key ) const { return _cache.at( p_key ).handle; }

		/**
		 * @brief Access descriptor by key.
		 */
		inline const D & descriptor( const K & p_key ) const { return _cache.at( p_key ).descriptor; }

		/**
		 * @brief Access resource by key.
		 */
		inline const T & get( const K & p_key ) const { return get( handle( p_key ) ); }

		inline T & get( const K & p_key ) { return get( handle( p_key ) ); }

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

			return *_resources[ p_handle ];
		}

		inline T & get( const Desc::Handle p_handle ) noexcept
		{
			assert( p_handle < _resources.size() );
			assert( std::find( _availables.begin(), _availables.end(), p_handle ) == _availables.end() );

			return *_resources[ p_handle ];
		}

		/**
		 * @brief Invalidate all resources.
		 */
		void invalidate()
		{
			_invalids.clear();
			for ( Desc::Handle handle = 0; handle < static_cast<Desc::Handle>( _resources.size() ); ++handle )
			{
				if ( std::find( _availables.begin(), _availables.end(), handle ) == _availables.end() )
				{
					_invalids.push_back( handle );
				}
			}
		}

		Desc::Handle getLastHandle() const { return static_cast<Desc::Handle>( _resources.size() - 1 ); }

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
		std::string toString()
		{
			std::string result = "ResourceHandler:\n";
			result += "  Total resources: " + std::to_string( _resources.size() ) + "\n";
			result += "  Available handles: " + std::to_string( _availables.size() ) + "\n";
			result += "  Invalid handles: " + std::to_string( _invalids.size() ) + "\n";
			return result;
		}

		/**
		 * @brief Iterator over key-value pairs.
		 */
		class iterator
		{
		  public:
			using VecIter = typename std::vector<std::unique_ptr<T>>::iterator;

			iterator( ResourceHandler * p_handler, VecIter p_it ) : _handler( p_handler ), _it( p_it )
			{ skipInvalid(); }

			iterator & operator++()
			{
				++_it;
				skipInvalid();
				return *this;
			}

			bool operator!=( const iterator & p_other ) const { return _it != p_other._it; }

			T & operator*() const { return **_it; }

		  private:
			void skipInvalid()
			{
				while ( _it != _handler->_resources.end() && *_it == nullptr )
				{
					++_it;
				}
			}

			ResourceHandler * _handler;
			VecIter			  _it;
		};

		iterator begin() { return iterator( this, _resources.begin() ); }

		iterator end() { return iterator( this, _resources.end() ); }

	  private:
		/**
		 * @brief Resource pool : Handle = index.
		 */
		std::vector<std::unique_ptr<T>> _resources;
		std::unordered_map<K, Entry>	_cache;

		/**
		 * @brief Available handles for reuse.
		 */
		std::vector<Desc::Handle> _availables;

		/**
		 * @brief Invalid handles.
		 */
		std::vector<Desc::Handle> _invalids;
	};

} // namespace VTX::Renderer

#endif
