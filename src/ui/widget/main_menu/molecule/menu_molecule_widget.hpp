#ifndef __VTX_UI_WIDGET_MENU_MOLECULE__
#define __VTX_UI_WIDGET_MENU_MOLECULE__

#include "ui/widget/main_menu/menu_tooltab_widget.hpp"
#include "ui/widget/main_menu/visualization/menu_visualization_object_display_widget.hpp"
#include "ui/widget/main_menu/visualization/menu_visualization_representation_widget.hpp"
#include "ui/widget/main_menu/visualization/menu_visualization_selection_action_widget.hpp"
#include "ui/widget/main_menu/visualization/menu_visualization_windows_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::MainMenu::Molecule
{
	class MenuMoleculeWidget : public MenuTooltabWidget
	{
		VTX_WIDGET

	  public:
		~MenuMoleculeWidget();
		void localize() override;

	  protected:
		MenuMoleculeWidget( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		Visualization::MenuVisualizationRepresentationWidget *	_representation	 = nullptr;
		Visualization::MenuVisualizationObjectDisplayWidget *	_objectDisplay	 = nullptr;
		Visualization::MenuVisualizationSelectionActionWidget * _selectionAction = nullptr;
		Visualization::MenuVisualizationWindowsWidget *			_windows		 = nullptr;
	};
} // namespace VTX::UI::Widget::MainMenu::Molecule
#endif
