#include "qt/ui_action/self_referenced_action.hpp"

namespace VTX::UI::QT::UIAction
{
	SelfReferencedAction::SelfReferencedAction( const QIcon & p_icon, const QString & p_text, QObject * p_parent ) :
		QAction( p_icon, p_text, p_parent )
	{
		connect( this, &SelfReferencedAction::triggered, this, &SelfReferencedAction::_callTriggeredSignal );
	}
	SelfReferencedAction::SelfReferencedAction( const QString & p_text, QObject * p_parent ) :
		QAction( p_text, p_parent )
	{
		connect( this, &SelfReferencedAction::triggered, this, &SelfReferencedAction::_callTriggeredSignal );
	}
	SelfReferencedAction::SelfReferencedAction( QObject * p_parent ) : QAction( p_parent )
	{
		connect( this, &SelfReferencedAction::triggered, this, &SelfReferencedAction::_callTriggeredSignal );
	}

	void SelfReferencedAction::_callTriggeredSignal() { emit triggeredSelf( this ); }

} // namespace VTX::UI::QT::UIAction
