#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_RESIDUE__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_RESIDUE__

#include "contextual_menu_template.hpp"
#include "ui/old_ui/ui/widget/custom_widget/set_representation_menu.hpp"
#include <QMenu>
#include <app/component/chemistry/residue.hpp>

namespace VTX::UI::Widget::ContextualMenu
{
	class ContextualMenuResidue : public ContextualMenuTemplate<App::Component::Chemistry::Residue>
	{
		VTX_WIDGET

	  public:
		~ContextualMenuResidue();
		void localize() override;
		void setTarget( App::Component::Chemistry::Residue * const p_target ) override;

	  protected:
		ContextualMenuResidue( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _orientAction();
		void _showAction();
		void _hideAction();
		void _soloAction();
		void _copyAction();
		void _extractAction();
		void _deleteAction();

		void _applyRepresentationAction( const int p_representationIndex );

	  private:
		CustomWidget::SetRepresentationMenu * _representationMenu;
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
