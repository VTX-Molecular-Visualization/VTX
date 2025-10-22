#ifndef __VTX_UI_QT_DOCK_WIDGET_COLOR_LAYOUTS__
#define __VTX_UI_QT_DOCK_WIDGET_COLOR_LAYOUTS__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include "ui/qt/settings.hpp"
#include <QCheckBox>

namespace VTX::UI::QT::DockWidget
{

	class ColorLayouts : public BaseDockWidget<>
	{
	  public:
		ColorLayouts( QWidget * );

		void save( Settings & ) override;
		void restore( const Settings & ) override;

	  private:
		inline static const QString _SETTING_KEY_HIDE = "colors/hide_non_common";

		QPointer<QCheckBox> _checkBoxHide;
	};

} // namespace VTX::UI::QT::DockWidget

#endif
