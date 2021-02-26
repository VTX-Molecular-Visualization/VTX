#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_MOLECULE__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "contextual_menu_template.hpp"
#include "model/molecule.hpp"
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

		void _orientAction();
		void _showAction();
		void _hideAction();
		void _copyAction();
		void _deleteAction();
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
