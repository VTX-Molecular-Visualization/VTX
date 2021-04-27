#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_OBJECT_DISPLAY__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_OBJECT_DISPLAY__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
#include "model/molecule.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QWidget>
#include <unordered_set>

namespace VTX::UI::Widget::MainMenu::Visualization
{
	class MenuVisualizationObjectDisplayWidget : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		~MenuVisualizationObjectDisplayWidget();
		void localize() override;

		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  protected:
		MenuVisualizationObjectDisplayWidget( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		// Show / Hide Molecular Objects
		MenuToolButtonWidget * _showWater	  = nullptr;
		MenuToolButtonWidget * _showSolvent	  = nullptr;
		MenuToolButtonWidget * _showHydrogens = nullptr;

		void _refreshButtons();

		void _fillContainerWithTarget( std::unordered_set<Model::Molecule *> & _container ) const;

		void _displayWaterButton( const bool p_active, const bool p_show );
		void _displaySolventButton( const bool p_active, const bool p_show );
		void _displayHydrogenButton( const bool p_active, const bool p_show );

		void _toggleWaterVisibilityAction() const;
		void _toggleSolventVisibilityAction() const;
		void _toggleHydrogenVisibilityAction() const;
	}; // namespace Visualization
} // namespace VTX::UI::Widget::MainMenu::Visualization
#endif
