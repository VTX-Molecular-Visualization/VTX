#ifndef __VTX_BASE_SELECTABLE__
#define __VTX_BASE_SELECTABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"

namespace VTX
{
	namespace Selection
	{
		class BaseSelectable
		{
		  public:
			inline bool	 isSelected() const { return _isSelected; }
			virtual void setSelected( const bool p_selected ) { _isSelected = p_selected; }

		  private:
			bool _isSelected = false;
		};
	} // namespace Selection
} // namespace VTX
#endif
