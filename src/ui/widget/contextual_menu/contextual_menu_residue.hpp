#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_RESIDUE__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_RESIDUE__

#ifdef _MSC_VER
#pragma once
#endif

#include "contextual_menu_template.hpp"
#include "model/residue.hpp"
#include <QMenu>

namespace VTX::UI::Widget::ContextualMenu
{
	class ContextualMenuResidue : public ContextualMenuTemplate<Model::Residue>
	{
		VTX_WIDGET

	  public:
		~ContextualMenuResidue();
		void localize() override;
		void setTarget( Model::Residue * const p_target ) override;

	  protected:
		ContextualMenuResidue( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _orientAction();
		void _showAction();
		void _hideAction();
		void _copyAction();
		void _extractAction();
		void _deleteAction();
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
