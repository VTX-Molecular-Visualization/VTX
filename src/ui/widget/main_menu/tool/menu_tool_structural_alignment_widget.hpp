#ifndef __VTX_UI_WIDGET_MENU_TOOL_STRUCTURAL_ALIGNMENT__
#define __VTX_UI_WIDGET_MENU_TOOL_STRUCTURAL_ALIGNMENT__

#include "event/event.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QString>
#include <QWidget>

namespace VTX::UI::Widget::MainMenu::Tool
{
	class MenuToolStructuralAlignmentWidget : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		~MenuToolStructuralAlignmentWidget();

		void localize() override;
		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  protected:
		MenuToolStructuralAlignmentWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		MenuToolButtonWidget * _rmsdButton					   = nullptr;
		MenuToolButtonWidget * _structuralAlignmentButton	   = nullptr;
		MenuToolButtonWidget * _structuralAlignmentPymolButton = nullptr;

		bool _checkRMSDEnableSate() const;
		bool _checkStructuralAlignmentEnableSate() const;

		void _computeRMSDAction() const;
		void _computeStructuralAlignmentAction() const;
		void _computeStructuralAlignmentPymolAction() const;

		void _refreshButtons() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Tool
#endif
