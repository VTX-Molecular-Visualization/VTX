#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_MOLECULE__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "contextual_menu_template.hpp"
#include "model/molecule.hpp"
#include "ui/widget/custom_widget/set_representation_menu.hpp"
#include <QMenu>

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
		void _toggleSolventVisibilityAction();
		void _toggleHydrogensVisibilityAction();

		void _orientAction();
		void _showAction();
		void _hideAction();
		void _copyAction();
		void _deleteAction();

		void _applyRepresentationAction( const int p_representationIndex );

		void _exportAction();

	  private:
		CustomWidget::SetRepresentationMenu * _representationMenu;
		QAction *							  _toggleWaterAction;
		QAction *							  _toggleSolventAction;
		QAction *							  _toggleHydrogenAction;
		QAction *							  _toggleTrajectoryPlayingAction;
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
