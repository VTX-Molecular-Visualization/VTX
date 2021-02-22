#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_INSTANTIATED_REPRESENTATION__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_INSTANTIATED_REPRESENTATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "contextual_menu_template.hpp"
#include "model/representation/instantiated_representation.hpp"
#include <QMenu>

namespace VTX::UI::Widget::ContextualMenu
{
	class ContextualMenuInstantiatedRepresentation :
		public ContextualMenuTemplate<Model::Representation::InstantiatedRepresentation>
	{
		VTX_WIDGET

	  public:
		~ContextualMenuInstantiatedRepresentation();
		void localize() override;
		void setTarget( Model::Representation::InstantiatedRepresentation * const p_target ) override;

	  protected:
		ContextualMenuInstantiatedRepresentation( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _deleteAction();
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
