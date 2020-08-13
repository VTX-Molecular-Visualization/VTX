#ifndef __VTX_BASE_DRAWABLE__
#define __VTX_BASE_DRAWABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_collectionable.hpp"
#include "base_visible.hpp"

namespace VTX
{
	namespace Generic
	{
		class BaseDrawable : virtual public BaseCollectionable, public BaseVisible
		{
		  public:
			virtual ~BaseDrawable() = default;

			virtual void draw()
			{
				if ( isVisible() )
				{
					_draw();
				}
			}

		  private:
			virtual void _draw() = 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
