#include "qt/actions.hpp"
#include <util/logger.hpp>

namespace VTX::UI::QT
{
	QAction * const ACTION( const Util::Hashing::Hash & p_hash, const UI::DescAction & p_action )
	{
		if ( not ACTIONS.contains( p_hash ) )
		{
			QAction * qAction = new QAction();
			ACTIONS[ p_hash ] = qAction;

			VTX_TRACE( "UI action created: {}", p_action.name );

			// Name.
			qAction->setText( p_action.name.c_str() );
			// Group.
			if ( p_action.group.has_value() )
			{
				std::string group = p_action.group.value();
				if ( not ACTION_GROUPS.contains( group ) )
				{
					ACTION_GROUPS[ group ] = new QActionGroup( nullptr );
				}

				qAction->setCheckable( true );
				ACTION_GROUPS[ group ]->addAction( qAction );
			}
			// Tip.
			if ( p_action.tip.has_value() )
			{
				QString tip = p_action.tip.value().c_str();

				if ( p_action.shortcut.has_value() )
				{
					tip.append( " (" + p_action.shortcut.value() + ")" );
				}

				qAction->setStatusTip( tip );
				qAction->setToolTip( tip );
				qAction->setWhatsThis( tip );
			}
			// Icon.
			if ( p_action.icon.has_value() )
			{
				qAction->setIcon( QIcon( p_action.icon.value().c_str() ) );
			}
			// Shortcut.
			if ( p_action.shortcut.has_value() )
			{
				qAction->setShortcut( QKeySequence( p_action.shortcut.value().c_str() ) );
			}
			// Action.
			if ( p_action.trigger.has_value() )
			{
				QObject::connect( qAction, &QAction::triggered, p_action.trigger.value() );
			}
		}

		return ACTIONS[ p_hash ];
	}
} // namespace VTX::UI::QT
