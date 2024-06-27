#ifndef __VTX_UI_QT_STATUS_BAR__
#define __VTX_UI_QT_STATUS_BAR__

#include <QStatusBar>
#include <ui/base_status_bar.hpp>

namespace VTX::UI::QT
{

	class StatusBar : public UI::BaseStatusBar, public QStatusBar
	{
	  public:
		StatusBar( QWidget * p_parent ) : BaseStatusBar(), QStatusBar( p_parent ) {}

	  private:
	};

} // namespace VTX::UI::QT

#endif
