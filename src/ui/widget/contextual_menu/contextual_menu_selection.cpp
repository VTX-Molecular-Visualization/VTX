#include "contextual_menu_selection.hpp"
#include "action/action_manager.hpp"
#include "action/selection.hpp"
#include "action/visible.hpp"

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuSelection::ContextualMenuSelection( QWidget * const p_parent ) : ContextualMenuTemplate( p_parent )
	{
		_actions.emplace_back( ActionData( "Orient", TypeMask::All, &ContextualMenuSelection::_orientAction ) );
		_actions.emplace_back( ActionData( "Show", TypeMask::AllButAtom, &ContextualMenuSelection::_showAction ) );
		_actions.emplace_back( ActionData( "Hide", TypeMask::AllButAtom, &ContextualMenuSelection::_hideAction ) );
		_actions.emplace_back( ActionData( "Copy", TypeMask::All, &ContextualMenuSelection::_copyAction ) );
		_actions.emplace_back( ActionData( "Extract", TypeMask::All, &ContextualMenuSelection::_extractAction ) );
		_actions.emplace_back(
			ActionData( "Delete", TypeMask::All, &ContextualMenuSelection::_deleteAction, QKeySequence::Delete ) );
	}
	ContextualMenuSelection ::~ContextualMenuSelection() {}

	void ContextualMenuSelection::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
	void ContextualMenuSelection::_setupSlots()
	{
		for ( const ActionData & actionData : _actions )
		{
			addAction( actionData.name, this, actionData.action, actionData.shortcut );
		}

		connect(
			this, &ContextualMenuSelection::aboutToShow, this, &ContextualMenuSelection::_updateActionsWithSelection );
	}

	void ContextualMenuSelection::localize() {}

	void ContextualMenuSelection::_updateActionsWithSelection()
	{
		std::set<ID::VTX_ID> typesInSelection = std::set<ID::VTX_ID>();
		_target->getItemTypes( typesInSelection );

		TypeMask selectionTypeMask = _getTypeMaskFromTypeSet( typesInSelection );

		for ( int i = 0; i < _actions.size(); i++ )
		{
			const ActionData & actionData	 = _actions[ i ];
			const bool		   actionVisible = bool( actionData.validTypes & selectionTypeMask );

			actions()[ i ]->setVisible( actionVisible );
		}
	}

	TypeMask ContextualMenuSelection::_getTypeMaskFromTypeSet( const std::set<ID::VTX_ID> & p_typeIds )
	{
		TypeMask res = TypeMask::None;

		if ( p_typeIds.find( ID::Model::MODEL_MOLECULE ) != p_typeIds.end() )
			res |= TypeMask::Molecule;
		if ( p_typeIds.find( ID::Model::MODEL_CHAIN ) != p_typeIds.end() )
			res |= TypeMask::Chain;
		if ( p_typeIds.find( ID::Model::MODEL_RESIDUE ) != p_typeIds.end() )
			res |= TypeMask::Residue;
		if ( p_typeIds.find( ID::Model::MODEL_ATOM ) != p_typeIds.end() )
			res |= TypeMask::Atom;

		return res;
	}

	void ContextualMenuSelection::_orientAction() { VTX_ACTION( new Action::Selection::Orient( *_target ) ); }
	void ContextualMenuSelection::_showAction()
	{
		VTX_ACTION( new Action::Selection::ChangeVisibility(
			*_target, Action::Visible::ChangeVisibility::VISIBILITY_MODE::ALL ) );
	}
	void ContextualMenuSelection::_hideAction()
	{
		VTX_ACTION( new Action::Selection::ChangeVisibility(
			*_target, Action::Visible::ChangeVisibility::VISIBILITY_MODE::HIDE ) );
	}
	void ContextualMenuSelection::_copyAction() { VTX_ACTION( new Action::Selection::Copy( *_target ) ); }
	void ContextualMenuSelection::_extractAction() { VTX_ACTION( new Action::Selection::Extract( *_target ) ); }
	void ContextualMenuSelection::_deleteAction() { VTX_ACTION( new Action::Selection::Delete( *_target ) ); }

} // namespace VTX::UI::Widget::ContextualMenu
