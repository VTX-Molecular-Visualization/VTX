#ifndef __VTX_UI_WIDGET_MENU_MOLECULE_OBJECT_DISPLAY_BLOCK__
#define __VTX_UI_WIDGET_MENU_MOLECULE_OBJECT_DISPLAY_BLOCK__

#include "old_ui/ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "old_ui/ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <QWidget>
#include <app/old_app/event/event.hpp>
#include <app/old_app/model/molecule.hpp>
#include <unordered_set>

namespace VTX::UI::Widget::MainMenu::Molecule
{
	class ObjectDisplayBlock : public MenuToolBlockWidget
	{
		VTX_WIDGET

	  public:
		~ObjectDisplayBlock();
		void localize() override;

		void receiveEvent( const VTX::Event::VTXEvent & p_event ) override;

	  protected:
		ObjectDisplayBlock( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		// Show / Hide Molecular Objects
		MenuToolButtonWidget * _showWater	  = nullptr;
		MenuToolButtonWidget * _showHydrogens = nullptr;
		MenuToolButtonWidget * _showSolvent	  = nullptr;
		MenuToolButtonWidget * _showIon		  = nullptr;

		void _refreshButtons();

		void _fillContainerWithTarget( std::unordered_set<Model::Molecule *> & _container ) const;

		void _displayWaterButton( const bool p_active, const bool p_show );
		void _displaySolventButton( const bool p_active, const bool p_show );
		void _displayHydrogenButton( const bool p_active, const bool p_show );
		void _displayIonButton( const bool p_active, const bool p_show );

		void _toggleWaterVisibilityAction() const;
		void _toggleSolventVisibilityAction() const;
		void _toggleHydrogenVisibilityAction() const;
		void _toggleIonVisibilityAction() const;
	}; // namespace Visualization
} // namespace VTX::UI::Widget::MainMenu::Molecule
#endif
