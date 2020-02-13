#ifndef __VTX_BASE_DRAWABLE__
#define __VTX_BASE_DRAWABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_collectionable.hpp"

namespace VTX
{
	namespace Generic
	{
		class BaseDrawable : virtual public BaseCollectionable
		{
		  public:
			virtual ~BaseDrawable() = default;

			virtual void draw()
			{
				if ( isVisible() ) { _draw(); }
			}

			inline bool	  isVisible() const { return _visible; }
			inline bool * isVisiblePtr() { return &_visible; }
			void		  setVisible( const bool p_visible ) { _visible = p_visible; }

		  protected:
			bool _visible = true;

		  private:
			virtual void _draw() = 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
