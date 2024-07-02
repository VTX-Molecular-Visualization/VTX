#ifndef __VTX_UI_QT_HELPER__
#define __VTX_UI_QT_HELPER__

#include <QAction>
#include <ui/descriptors.hpp>

namespace VTX::UI::QT::Helper
{

	static QAction * const toQAction( const UI::Action & p_action )
	{
		QAction * const action = new QAction();

		action->setText( p_action.name.c_str() );
		if ( p_action.tip.has_value() )
		{
			action->setStatusTip( p_action.tip.value().c_str() );
			action->setToolTip( p_action.tip.value().c_str() );
			action->setWhatsThis( p_action.tip.value().c_str() );
		}
		if ( p_action.icon.has_value() )
		{
			action->setIcon( QIcon( p_action.icon.value().c_str() ) );
		}

		return action;
	}

	static QAction * addQAction( QWidget * const p_parent, const UI::Action & p_action )
	{
		QAction * const action = toQAction( p_action );
		p_parent->addAction( action );

		// Connect signal.
		// QObject::connect( action, &QAction::triggered, p_action.callback );

		return action;
	}

} // namespace VTX::UI::QT::Helper

#endif
