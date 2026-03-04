#ifndef __VTX_RENDERER_LAYOUT_BASE_LAYOUT__
#define __VTX_RENDERER_LAYOUT_BASE_LAYOUT__

#include "renderer/context/context_wrapper.hpp"

namespace VTX::Renderer::Layout
{
	/**
	 * @brief Base layout struct to handle attribute buffers.
	 */
	class BaseLayout : public Desc::VertexLayout
	{
	  public:
		/**
		 * @brief Push a range.
		 */
		inline void add( const SystemUID p_uid, const IndexRange & p_range ) { _ranges[ p_uid ] = p_range; }

		/**
		 * @brief Resize whole layout.
		 */
		inline void resize( Context::ContextWrapper & p_context )
		{
			Index size = 0;
			for ( const auto & [ uid, range ] : _ranges )
			{
				size += range.getCount();
			}

			_resize( p_context, size );
		}

		/**
		 * @brief Accessors.
		 */
		// inline Index size() const { return _size; }

	  protected:
		/**
		 * @brief Range to draw per system (global indexes).
		 */
		mutable MapUIDRange _ranges;

		/**
		 * @brief Current size.
		 */
		// Index _size = 0;

		/**
		 * @brief Resize whole layout.
		 */
		virtual void _resize( Context::ContextWrapper &, const Index ) = 0;
	};
} // namespace VTX::Renderer::Layout

#endif
