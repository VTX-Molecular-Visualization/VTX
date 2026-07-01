#ifndef __VTX_UI_QT_DOCK_WIDGET_DEBUG__
#define __VTX_UI_QT_DOCK_WIDGET_DEBUG__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include <QLabel>
#include <QPointer>

namespace VTX::UI::QT::DockWidget
{

	class Debug : public BaseDockWidget<Debug>
	{

	  public:
		Debug( QWidget * p_parent );
		~Debug();

	  private:
		QPointer<QLabel>	_labelCacheCount;
		QPointer<QLabel>	_labelCacheSize;

	};

} // namespace VTX::UI::QT::DockWidget

#endif