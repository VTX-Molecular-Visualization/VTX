#ifndef __VTX_RENDERER_CONTEXT_HANDLE_MANAGER__
#define __VTX_RENDERER_CONTEXT_HANDLE_MANAGER__

#include "renderer/descriptors.hpp"
#include <algorithm>
#include <vector>

namespace VTX::Renderer::Context
{
	/**
	 * @brief Generic resource handler.
	 * Store resources and provide access through handles.
	 */
	template<typename T>
	class ResourceHandler
	{
	  public:
		/**
		 * @brief Emplace a new resource.
		 * Reuse available handles if any.
		 */
		template<typename... Args>
		Desc::Handle emplace( const Desc::Key p_key, Args &&... p_args )
		{
			Desc::Handle handle;
			// Reuse available handle if any.
			if ( not _availables.empty() )
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
			_keyHandles.insert_or_assign( p_key, handle );

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
		void erase( const Desc::Key p_key )
		{
			if ( not _keyHandles.contains( p_key ) )
			{
				return;
			}

			erase( at( p_key ) );
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
				_keyHandles.begin(), _keyHandles.end(), [ & ]( const auto & pair ) { return pair.second == p_handle; }
			);
			if ( it != _keyHandles.end() )
			{
				_keyHandles.erase( it );
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
			_keyHandles.clear();
			_availables.clear();
		}

		/**
		 * @brief Check if a resource exists from key.
		 */
		inline bool contains( const Desc::Key p_key ) const
		{
			if ( not _keyHandles.contains( p_key ) )
			{
				return false;
			}

			return contains( at( p_key ) );
		}

		inline bool contains( const Desc::Handle p_handle ) const noexcept
		{
			return p_handle < _resources.size() && _resources[ p_handle ];
		}

		/**
		 * @brief Check if a resource exists from key and remove from invalids if present.
		 */
		inline bool validate( const Desc::Key p_key )
		{
			if ( not _keyHandles.contains( p_key ) )
			{
				return false;
			}

			const Desc::Handle handle = at( p_key );

			return validate( handle );
		}

		inline bool validate( const Desc::Handle p_handle ) noexcept
		{
			std::erase( _invalids, p_handle );
			return contains( p_handle );
		}

		/**
		 * @brief Access handle by key.
		 */
		inline Desc::Handle at( const Desc::Key p_key ) const { return _keyHandles.at( p_key ); }

		/**
		 * @brief Access resource by key.
		 */
		inline const T & get( const Desc::Key p_key ) const { return get( at( p_key ) ); }
		inline T &		 get( const Desc::Key p_key ) { return get( at( p_key ) ); }

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

	  private:
		/**
		 * @brief Resource pool : Handle = index.
		 */
		std::vector<std::unique_ptr<T>> _resources;

		/**
		 * @brief Cache : mapping Key -> Handle.
		 */
		std::unordered_map<Desc::Key, Desc::Handle> _keyHandles;

		/**
		 * @brief Available handles for reuse.
		 */
		std::vector<Desc::Handle> _availables;

		/**
		 * @brief Invalid handles.
		 */
		std::vector<Desc::Handle> _invalids;
	};

} // namespace VTX::Renderer::Context

#endif
