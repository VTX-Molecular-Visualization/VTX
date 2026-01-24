#ifndef __VTX_RENDERER_CONTEXT_HANDLE_MANAGER__
#define __VTX_RENDERER_CONTEXT_HANDLE_MANAGER__

#include "renderer/descriptors.hpp"
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
		void add() {}

		void emplace() {}

		void remove( const Desc::Key ) {}
		void remove( const Desc::Handle ) {}

		const T & get( const Desc::Handle p_handle ) const noexcept
		{
			assert( p_handle < _resources.size() );
			assert( std::find( _availables.begin(), _availables.end(), p_handle ) == _availables.end() );

			return *_resources[ p_handle ];
		}

		T & get( const Desc::Handle p_handle ) noexcept
		{
			assert( p_handle < _resources.size() );
			assert( std::find( _availables.begin(), _availables.end(), p_handle ) == _availables.end() );

			return *_resources[ p_handle ];
		}

		void invalidate() {}

		void purge() {}

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
	};

} // namespace VTX::Renderer::Context

#endif
