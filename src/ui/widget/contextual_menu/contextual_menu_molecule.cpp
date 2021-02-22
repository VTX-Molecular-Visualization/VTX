#include "contextual_menu_molecule.hpp"
#include "action/action_manager.hpp"
#include "action/molecule.hpp"
#include "action/visible.hpp"

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuMolecule::ContextualMenuMolecule( QWidget * const p_parent ) : ContextualMenuTemplate( p_parent ) {}
	ContextualMenuMolecule ::~ContextualMenuMolecule() {}

	void ContextualMenuMolecule::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
	void ContextualMenuMolecule::_setupSlots()
	{
		addAction( "Orient", this, &ContextualMenuMolecule::_orientAction );
		addAction( "Show", this, &ContextualMenuMolecule::_showAction );
		addAction( "Hide", this, &ContextualMenuMolecule::_hideAction );
		addAction( "Copy", this, &ContextualMenuMolecule::_copyAction );
		addAction( "Delete", this, &ContextualMenuMolecule::_deleteAction, QKeySequence::Delete );
	}

	void ContextualMenuMolecule::localize() {}

	void ContextualMenuMolecule::setTarget( Model::Molecule * const p_target )
	{
		ContextualMenuTemplate::setTarget( p_target );
		setTitle( QString::fromStdString( p_target->getPdbIdCode() ) );
	}

	void ContextualMenuMolecule::_orientAction() { VTX_ACTION( new Action::Molecule::Orient( *_target ) ); }
	void ContextualMenuMolecule::_showAction()
	{
		VTX_ACTION( new Action::Molecule::ChangeVisibility( *_target,
															Action::Visible::ChangeVisibility::VISIBILITY_MODE::ALL ) );
	}
	void ContextualMenuMolecule::_hideAction()
	{
		VTX_ACTION( new Action::Molecule::ChangeVisibility(
			*_target, Action::Visible::ChangeVisibility::VISIBILITY_MODE::HIDE ) );
	}
	void ContextualMenuMolecule::_copyAction() { VTX_ACTION( new Action::Molecule::Copy( *_target ) ); }
	void ContextualMenuMolecule::_deleteAction() { VTX_ACTION( new Action::Molecule::Delete( *_target ) ); }

} // namespace VTX::UI::Widget::ContextualMenu
