#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_ATOM__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_ATOM__

#include "contextual_menu_template.hpp"
#include <QMenu>
#include <app/component/chemistry/atom.hpp>

namespace VTX::UI::Widget::ContextualMenu
{
	class ContextualMenuAtom : public ContextualMenuTemplate<App::Component::Chemistry::Atom>
	{
		VTX_WIDGET

	  public:
		~ContextualMenuAtom();
		void localize() override;
		void setTarget( App::Component::Chemistry::Atom * const p_target ) override;

	  protected:
		ContextualMenuAtom( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _orientAction();
		void _showAction();
		void _hideAction();
		void _soloAction();
		void _copyAction();
		void _extractAction();
		void _deleteAction();
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
