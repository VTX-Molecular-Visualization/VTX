#ifndef __VTX_BASE_VISIBLE__
#define __VTX_BASE_VISIBLE__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Generic
	{
		class BaseVisible
		{
		  public:
			inline bool	  isVisible() const { return _visible; }
			inline bool * isVisiblePtr() { return &_visible; }
			void		  setVisible( const bool p_visible ) { _visible = p_visible; }

		  protected:
			bool _visible = true;
		};
	} // namespace Generic
} // namespace VTX
#endif
