#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_MOLECULE__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_MOLECULE__

#include "contextual_menu_template.hpp"
#include "old_ui/ui/widget/custom_widget/set_representation_menu.hpp"
#include <QMenu>
#include <app/old_app/model/molecule.hpp>

namespace VTX::UI::Widget::ContextualMenu
{
	class ContextualMenuMolecule : public ContextualMenuTemplate<Model::Molecule>
	{
		VTX_WIDGET

	  public:
		~ContextualMenuMolecule();
		void localize() override;
		void setTarget( Model::Molecule * const p_target ) override;

	  protected:
		ContextualMenuMolecule( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _renameAction();

		void _toggleTrajectoryPlayingActions();

		void _toggleWatersVisibilityAction();
		void _toggleHydrogensVisibilityAction();
		void _toggleSolventVisibilityAction();
		void _toggleIonVisibilityAction();

		void _orientAction();
		void _showAction();
		void _hideAction();
		void _soloAction();
		void _copyAction();
		void _deleteAction();

		void _applyRepresentationAction( const int p_representationIndex );

		void _exportAction();
		void _loadTrajectoryAction();

	  private:
		CustomWidget::SetRepresentationMenu * _representationMenu = nullptr;

		QAction * _toggleWaterAction			 = nullptr;
		QAction * _toggleHydrogenAction			 = nullptr;
		QAction * _toggleSolventAction			 = nullptr;
		QAction * _toggleIonAction				 = nullptr;
		QAction * _toggleTrajectoryPlayingAction = nullptr;
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
