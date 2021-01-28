#ifndef __VTX_UI_WIDGET_SETTING_MENU_ITEM__
#define __VTX_UI_WIDGET_SETTING_MENU_ITEM__

#ifdef _MSC_VER
#pragma once
#endif

#include "setting_widget_enum.hpp"
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QWidget>

namespace VTX::UI::Widget::Settings
{
	class SettingWidget;

	class SettingMenuItem : public QObject
	{
	  public:
		SettingMenuItem( SettingWidget * const p_parent,
						 const SETTING_MENU	   p_menu,
						 QWidget * const	   p_content,
						 const QString &	   p_title );

		QPushButton * const getButton() { return _button; }
		QWidget * const		getContent() { return _content; }
		const QString &		getTitle() const { return _title; }

		void setVisible( bool p_visible );

	  private:
		SettingWidget * const _parent = nullptr;
		const SETTING_MENU	  _menu;
		const QString		  _title   = "";
		QWidget * const		  _content = nullptr;
		QPushButton *		  _button  = nullptr;

		void _onButtonPushed();
	};
} // namespace VTX::UI::Widget::Settings
#endif
