#ifndef __VTX_UI_QT_DOCK_WIDGET_COLOR_LAYOUTS__
#define __VTX_UI_QT_DOCK_WIDGET_COLOR_LAYOUTS__

#include "ui/qt/core/base_dock_widget.hpp"
#include "ui/qt/settings.hpp"
#include <QCheckBox>
#include <QDockWidget>

namespace VTX::UI::QT::DockWidget
{

	class ColorLayouts : public Core::BaseDockWidget<ColorLayouts>, public Savable
	{
	  public:
		ColorLayouts( QWidget * );

		void save() override;
		void restore() override;

	  private:
		inline static const QString _SETTING_KEY_HIDE = "colors/hide_non_common";
		inline static const int		_BUTTON_SIZE	  = 32;

		QPointer<QCheckBox> _checkBoxHide;
	};

} // namespace VTX::UI::QT::DockWidget

#endif
