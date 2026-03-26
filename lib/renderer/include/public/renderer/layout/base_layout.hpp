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
		inline void add( const Desc::Handle p_handle, const Index & p_count )
		{
			if ( _ranges.contains( p_handle ) )
			{
				return;
			}

			Index	   last		= _rangeList.isEmpty() ? 0 : _rangeList.getLast();
			IndexRange r		= IndexRange::fromFirstCount( last, p_count );
			_ranges[ p_handle ] = r;
			_rangeList.addRange( r );
		}

		/**
		 * @brief Clear all registered ranges.
		 */
		inline void clear()
		{
			_ranges.clear();
			_rangeList.clear();
		}

		/**
		 * @brief Resize whole layout.
		 */
		inline void resize( Context::ContextWrapper & p_context )
		{
			const Index size = _rangeList.count();
			_resize( p_context, size == 0 ? 1 : size );
		}

		/**
		 * @brief Get offset.
		 */
		Index offset( const Desc::Handle p_handle ) const
		{
			assert( _ranges.contains( p_handle ) );

			return _ranges[ p_handle ].getFirst();
		}

		/**
		 * @brief Get size.
		 */
		Index size( const Desc::Handle p_handle ) const
		{
			assert( _ranges.contains( p_handle ) );

			return _ranges[ p_handle ].getCount();
		}

		/**
		 * @brief Resize whole layout.
		 */
		virtual void _resize( Context::ContextWrapper &, const Index ) = 0;

	  private:
		/**
		 * @brief Range to draw per system (global indexes).
		 */
		mutable std::unordered_map<Desc::Handle, IndexRange> _ranges;
		IndexRangeList										 _rangeList;
	};
} // namespace VTX::Renderer::Layout

#endif
