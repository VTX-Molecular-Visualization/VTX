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
		inline void add( const SystemUID p_uid, const Index & p_count )
		{
			if ( _ranges.contains( p_uid ) )
			{
				return;
			}

			Index	   last	 = _rangeList.isEmpty() ? 0 : _rangeList.getLast();
			IndexRange r	 = IndexRange::fromFirstCount( last, p_count );
			_ranges[ p_uid ] = r;
			_rangeList.addRange( r );
		}

		/**
		 * @brief Resize whole layout.
		 */
		inline void resize( Context::ContextWrapper & p_context ) { _resize( p_context, _rangeList.count() ); }

		Index size( const SystemUID p_uid ) const
		{
			assert( _ranges.contains( p_uid ) );

			return _ranges[ p_uid ].getCount();
		}

	  protected:
		/**
		 * @brief Range to draw per system (global indexes).
		 */
		mutable MapUIDRange _ranges;
		IndexRangeList		_rangeList;

		/**
		 * @brief Resize whole layout.
		 */
		virtual void _resize( Context::ContextWrapper &, const Index ) = 0;
	};
} // namespace VTX::Renderer::Layout

#endif
