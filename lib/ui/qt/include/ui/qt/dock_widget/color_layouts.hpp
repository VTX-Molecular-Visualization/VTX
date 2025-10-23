#ifndef __VTX_UI_QT_DOCK_WIDGET_COLOR_LAYOUTS__
#define __VTX_UI_QT_DOCK_WIDGET_COLOR_LAYOUTS__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include <QCheckBox>

namespace VTX::UI::QT::DockWidget
{

	class ColorLayouts : public BaseDockWidget<ColorLayouts>
	{
	  public:
		ColorLayouts( QWidget * );
		~ColorLayouts();

	  private:
		QPointer<QCheckBox> _checkBoxHide;
	};

} // namespace VTX::UI::QT::DockWidget

#endif
