#ifndef __VTX_UI_QT_DOCK_WIDGET_OPTIONS__
#define __VTX_UI_QT_DOCK_WIDGET_OPTIONS__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QPointer>

namespace VTX::UI::QT::DockWidget
{

	class Options : public BaseDockWidget<Options>
	{
	  public:
		Options( QWidget * p_parent );
		~Options();

	  private:
		QPointer<QComboBox> _comboBoxTheme;
		QPointer<QComboBox> _comboBoxFont;
		QPointer<QCheckBox> _checkBoxVSync;
		QPointer<QLabel>	_labelCacheCount;
		QPointer<QLabel>	_labelCacheSize;

		void _refreshCacheInfos();
	};

} // namespace VTX::UI::QT::DockWidget

#endif
