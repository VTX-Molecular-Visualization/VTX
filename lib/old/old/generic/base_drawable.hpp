#ifndef __VTX_BASE_DRAWABLE__
#define __VTX_BASE_DRAWABLE__

#include "base_visible.hpp"

namespace VTX
{
	namespace Generic
	{
		class BaseDrawable : public BaseVisible
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
