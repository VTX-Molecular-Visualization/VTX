#ifndef __VTX_UI_WIDGET_SETTING__
#define __VTX_UI_WIDGET_SETTING__

#ifdef _MSC_VER
#pragma once
#endif

#include "setting_color_widget.hpp"
#include "setting_render_effect_widget.hpp"
#include "setting_representation_widget.hpp"
#include "setting_widget_enum.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/base_manual_widget_initializer.hpp"
#include "ui/widget/custom_widget/dock_window_main_widget.hpp"
#include <QDockWidget>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QTabWidget>

namespace VTX::UI::Widget::Settings
{
	class SettingMenuItem;

	class SettingWidget : public BaseManualWidget<QDockWidget>
	{
		VTX_WIDGET

	  public:
		void receiveEvent( const Event::VTXEvent & p_event ) override;
		void localize() override;

		void setCurrentMenu( const SETTING_MENU & p_menuIndex, const bool p_force = false );

	  protected:
		SettingWidget( QWidget * const );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _addItem( QWidget * const p_itemContent, const SETTING_MENU & p_menu, const QString & p_title );

	  private:
		SETTING_MENU _currentMenu = SETTING_MENU::REPRESENTATIONS;

		std::vector<SettingMenuItem *> _menus = std::vector<SettingMenuItem *>();

		CustomWidget::DockWindowMainWidget<QWidget> * _menu = nullptr;

		QVBoxLayout * _menuLayout	 = nullptr;
		QVBoxLayout * _contentLayout = nullptr;

		QLabel * _title = nullptr;
	};
} // namespace VTX::UI::Widget::Settings
#endif
