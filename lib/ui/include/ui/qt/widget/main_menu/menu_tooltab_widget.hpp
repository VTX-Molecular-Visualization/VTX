#ifndef __VTX_UI_QT_WIDGET_MENU_TOOLTAB__
#define __VTX_UI_QT_WIDGET_MENU_TOOLTAB__

#include "ui/core/main_menu/main_menu_tooltab.hpp"
#include "ui/qt/widget/base_manual_widget.hpp"
#include "menu_toolblock_widget.hpp"
#include <QHBoxLayout>
#include <QString>
#include <QWidget>

namespace VTX::UI::QT::Widget::MainMenu
{
	class MenuTooltabWidget : public VTX::UI::Core::MainMenu::MainMenuTooltab, public BaseManualWidget<QWidget>
	{
		NEW_ARCHI_VTX_WIDGET

	  public:
		~MenuTooltabWidget();

		void localize() override {};

	  protected:
		MenuTooltabWidget( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override {};

		virtual VTX::UI::Core::MainMenu::MainMenuToolBlock * _instantiateBlock() override;
		void												 _addSeparator() override;

	  private:
		QHBoxLayout * _horizontalLayout = nullptr;
	};
} // namespace VTX::UI::QT::Widget::MainMenu
#endif
