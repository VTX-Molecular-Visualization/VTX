#ifndef __VTX_UI_QT_DOCK_WIDGET_OPTIONS__
#define __VTX_UI_QT_DOCK_WIDGET_OPTIONS__

#include "ui/qt/dock_widget/base_dock_widget.hpp"
#include "ui/qt/events.hpp"
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QPointer>

namespace VTX::UI::QT::DockWidget
{

	class Options : public BaseDockWidget<Options>
	{
		/**
		 * @brief Default values.
		 */
		static constexpr bool SAVE_POWER_DEFAULT = true;

	  public:
		Options( QWidget * p_parent );
		~Options();

	  private:
		QPointer<QComboBox> _comboBoxTheme;
		QPointer<QComboBox> _comboBoxFont;
		QPointer<QComboBox> _comboBoxKBLayout;
		QPointer<QCheckBox> _checkBoxVSync;
		QPointer<QCheckBox> _checkBoxSavePower;
		QPointer<QLabel>	_labelCacheCount;
		QPointer<QLabel>	_labelCacheSize;

		void _syncThemeComboBox();
		void _onThemeChanged( const Events::ThemeChanged & );
		void _refreshCacheInfos();
	};

} // namespace VTX::UI::QT::DockWidget

#endif
