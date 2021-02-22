#include "contextual_menu_chain.hpp"
#include "action/action_manager.hpp"
#include "action/chain.hpp"
#include "action/visible.hpp"

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuChain::ContextualMenuChain( QWidget * const p_parent ) : ContextualMenuTemplate( p_parent ) {}
	ContextualMenuChain ::~ContextualMenuChain() {}

	void ContextualMenuChain::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
	void ContextualMenuChain::_setupSlots()
	{
		addAction( "Orient", this, &ContextualMenuChain::_orientAction );
		addAction( "Show", this, &ContextualMenuChain::_showAction );
		addAction( "Hide", this, &ContextualMenuChain::_hideAction );
		addAction( "Copy", this, &ContextualMenuChain::_copyAction );
		addAction( "Extract", this, &ContextualMenuChain::_extractAction );
		addAction( "Delete", this, &ContextualMenuChain::_deleteAction, QKeySequence::Delete );
	}

	void ContextualMenuChain::localize() {}

	void ContextualMenuChain::setTarget( Model::Chain * const p_target )
	{
		ContextualMenuTemplate::setTarget( p_target );

		const std::string & title = p_target->getName();
		setTitle( QString::fromStdString( title ) );
	}

	void ContextualMenuChain::_orientAction() { VTX_ACTION( new Action::Chain::Orient( *_target ) ); }
	void ContextualMenuChain::_showAction()
	{
		VTX_ACTION(
			new Action::Chain::ChangeVisibility( *_target, Action::Visible::ChangeVisibility::VISIBILITY_MODE::ALL ) );
	}
	void ContextualMenuChain::_hideAction()
	{
		VTX_ACTION(
			new Action::Chain::ChangeVisibility( *_target, Action::Visible::ChangeVisibility::VISIBILITY_MODE::HIDE ) );
	}
	void ContextualMenuChain::_copyAction() { VTX_ACTION( new Action::Chain::Copy( *_target ) ); }
	void ContextualMenuChain::_extractAction() { VTX_ACTION( new Action::Chain::Extract( *_target ) ); }

	void ContextualMenuChain::_deleteAction() { VTX_ACTION( new Action::Chain::Delete( *_target ) ); }

} // namespace VTX::UI::Widget::ContextualMenu
