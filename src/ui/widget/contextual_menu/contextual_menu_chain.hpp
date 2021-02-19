#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_CHAIN__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_CHAIN__

#ifdef _MSC_VER
#pragma once
#endif

#include "contextual_menu_template.hpp"
#include "model/chain.hpp"
#include <QMenu>

namespace VTX::UI::Widget::ContextualMenu
{
	class ContextualMenuChain : public ContextualMenuTemplate<Model::Chain>
	{
		VTX_WIDGET

	  public:
		~ContextualMenuChain();
		void localize() override;
		void setTarget( Model::Chain * const p_target ) override;

	  protected:
		ContextualMenuChain( QWidget * p_parent = nullptr );
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
