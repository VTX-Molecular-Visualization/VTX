#ifndef __VTX_UI_WIDGET_MENU_TOOL__
#define __VTX_UI_WIDGET_MENU_TOOL__

#include "menu_tool_measurement_widget.hpp"
#include "menu_tool_structural_alignment_widget.hpp"
#include <QWidget>
#include <ui/old_ui/ui/widget/main_menu/menu_tooltab_widget.hpp>

namespace VTX::UI::Widget::MainMenu::Tool
{
	class MenuToolWidget : public MenuTooltabWidget
	{
		VTX_WIDGET

	  public:
		~MenuToolWidget();
		void localize() override;

	  protected:
		MenuToolWidget( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		MenuToolMeasurementWidget *			_measurement		 = nullptr;
		MenuToolStructuralAlignmentWidget * _structuralAlignment = nullptr;
	};
} // namespace VTX::UI::Widget::MainMenu::Tool
#endif
