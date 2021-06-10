#include "contextual_menu_atom.hpp"
#include "action/action_manager.hpp"
#include "action/atom.hpp"
#include "action/visible.hpp"

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuAtom::ContextualMenuAtom( QWidget * const p_parent ) : ContextualMenuTemplate( p_parent ) {}
	ContextualMenuAtom ::~ContextualMenuAtom() {}

	void ContextualMenuAtom::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
	void ContextualMenuAtom::_setupSlots()
	{
		addAction( "Orient", this, &ContextualMenuAtom::_orientAction );
		addAction( "Show", this, &ContextualMenuAtom::_showAction );
		addAction( "Hide", this, &ContextualMenuAtom::_hideAction );
		addAction( "Copy", this, &ContextualMenuAtom::_copyAction );
		addAction( "Extract", this, &ContextualMenuAtom::_extractAction );
		addAction( "Delete", this, &ContextualMenuAtom::_deleteAction );
	}

	void ContextualMenuAtom::localize() {}

	void ContextualMenuAtom::setTarget( Model::Atom * const p_target )
	{
		ContextualMenuTemplate::setTarget( p_target );

		const std::string & title = p_target->getName();
		setTitle( QString::fromStdString( title ) );
	}

	void ContextualMenuAtom::_orientAction() { VTX_ACTION( new Action::Atom::Orient( *_target ) ); }
	void ContextualMenuAtom::_showAction()
	{
		VTX_ACTION(
			new Action::Atom::ChangeVisibility( *_target, Action::Visible::ChangeVisibility::VISIBILITY_MODE::ALL ) );
	}
	void ContextualMenuAtom::_hideAction()
	{
		VTX_ACTION(
			new Action::Atom::ChangeVisibility( *_target, Action::Visible::ChangeVisibility::VISIBILITY_MODE::HIDE ) );
	}
	void ContextualMenuAtom::_copyAction() { VTX_ACTION( new Action::Atom::Copy( *_target ) ); }
	void ContextualMenuAtom::_extractAction() { VTX_ACTION( new Action::Atom::Extract( *_target ) ); }
	void ContextualMenuAtom::_deleteAction() { VTX_ACTION( new Action::Atom::Delete( *_target ) ); }

} // namespace VTX::UI::Widget::ContextualMenu
