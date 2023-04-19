#ifndef __VTX_UI_WIDGET_MENU_TOOL_STRUCTURAL_ALIGNMENT__
#define __VTX_UI_WIDGET_MENU_TOOL_STRUCTURAL_ALIGNMENT__

#include "tool/old_tool/analysis/structural_alignment.hpp"
#include <QString>
#include <QWidget>
#include <app/event/vtx_event.hpp>
#include <ui/old_ui/ui/widget/main_menu/menu_toolblock_widget.hpp>
#include <ui/old_ui/ui/widget/main_menu/menu_toolbutton_submenu_widget.hpp>
#include <ui/old_ui/ui/widget/main_menu/menu_toolbutton_widget.hpp>
#include <vector>

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class Selection;
	} // namespace Model

	namespace UI::Widget::MainMenu::Tool
	{
		class MenuToolStructuralAlignmentWidget : public MenuToolBlockWidget
		{
			VTX_WIDGET

		  public:
			~MenuToolStructuralAlignmentWidget();

			void localize() override;
			void receiveEvent( const VTX::Event::VTXEvent & p_event ) override;

		  protected:
			MenuToolStructuralAlignmentWidget( QWidget * p_parent );
			void _setupUi( const QString & p_name ) override;
			void _setupSlots() override;

		  private:
			MenuToolButtonWidget *		  _rmsdButton						 = nullptr;
			MenuToolButtonSubmenuWidget * _structuralAlignmentButton		 = nullptr;
			MenuToolButtonWidget *		  _structuralAlignmentAdvancedButton = nullptr;

			VTX::Analysis::StructuralAlignment::AlignmentParameters * _alignmentParameter = nullptr;

			bool _checkRMSDEnableSate() const;
			bool _checkStructuralAlignmentEnableSate() const;

			void _computeRMSDAction() const;
			void _computeStructuralAlignmentAction();
			void _launchStructuralAlignmentAction() const;

			void _openStructuralAlignmentWindow() const;

			void _refreshButtons() const;
		};
	} // namespace UI::Widget::MainMenu::Tool
} // namespace VTX
#endif
