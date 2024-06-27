#ifndef __VTX_UI_QT_STATUS_BAR__
#define __VTX_UI_QT_STATUS_BAR__

#include "qt/base_widget.hpp"
#include <QStatusBar>

namespace VTX::UI::QT
{

	class StatusBar : public BaseWidget<StatusBar, QStatusBar>
	{
	  public:
		StatusBar( QWidget * p_parent ) : BaseWidget<StatusBar, QStatusBar>( p_parent ) {}
		virtual ~StatusBar() {}

	  private:
	};

} // namespace VTX::UI::QT

#endif
