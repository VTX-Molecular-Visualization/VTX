#ifndef __VTX_UI_WIDGET_BASE_MENUTOOLBLOCK_WIDGET__
#define __VTX_UI_WIDGET_BASE_MENUTOOLBLOCK_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_widget.hpp"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			template<typename Q, typename T>
			class BaseMenuToolBlockWidget : public BaseWidget<Q, T>
			{
			  public:
				BaseMenuToolBlockWidget( QWidget * p_parent ) : BaseWidget( p_parent ) {};
			};
		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
#endif
