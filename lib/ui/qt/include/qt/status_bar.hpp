#ifndef __VTX_UI_QT_STATUS_BAR__
#define __VTX_UI_QT_STATUS_BAR__

#include <QStatusBar>

namespace VTX::UI::QT
{

	class StatusBar : public QStatusBar
	{
	  public:
		StatusBar( QWidget * p_parent ) : QStatusBar( p_parent ) {}
		virtual ~StatusBar() {}

	  private:
	};

} // namespace VTX::UI::QT

#endif
