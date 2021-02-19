#include "contextual_menu_instantiated_representation.hpp"
#include "action/action_manager.hpp"
#include "action/representable.hpp"
#include "action/visible.hpp"

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuInstantiatedRepresentation::ContextualMenuInstantiatedRepresentation( QWidget * const p_parent ) :
		ContextualMenuTemplate( p_parent )
	{
	}
	ContextualMenuInstantiatedRepresentation ::~ContextualMenuInstantiatedRepresentation() {}

	void ContextualMenuInstantiatedRepresentation::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
	}
	void ContextualMenuInstantiatedRepresentation::_setupSlots()
	{
		addAction( "Delete", this, &ContextualMenuInstantiatedRepresentation::_deleteAction, QKeySequence::Delete );
	}

	void ContextualMenuInstantiatedRepresentation::localize() {}

	void ContextualMenuInstantiatedRepresentation::setTarget(
		Model::Representation::InstantiatedRepresentation * const p_target )
	{
		ContextualMenuTemplate::setTarget( p_target );

		const std::string & title = p_target->getName();
		setTitle( QString::fromStdString( title ) );
	}

	void ContextualMenuInstantiatedRepresentation::_deleteAction()
	{
		VTX_ACTION( new Action::DeleteRepresentation( _target ) );
	}

} // namespace VTX::UI::Widget::ContextualMenu
