#include "qt/core/contextual_menu/contextual_menu_template.hpp"

namespace VTX::UI::QT::Core::ContextualMenu
{
	QAction * BaseContextualMenu::addSection(
		const QString &				  p_sectionName,
		const SECTION_POSITION_HINT & p_positionHint
	)
	{
		QAction * const before = _getNextSection( p_positionHint );
		QAction * const action = QMenu::insertSection( before, p_sectionName );

		action->setData( QVariant( int( p_positionHint ) ) );

		return action;
	}

	QAction * BaseContextualMenu::_getNextSection( const SECTION_POSITION_HINT & p_positionHint ) const
	{
		QAction * res = nullptr;

		switch ( p_positionHint )
		{
		case SECTION_POSITION_HINT::TOP:
		{
			for ( QAction * action : actions() )
			{
				if ( action->isSeparator() && action->data().isValid() )
				{
					const SECTION_POSITION_HINT sectionPositionHint = SECTION_POSITION_HINT( action->data().toInt() );
					if ( sectionPositionHint == SECTION_POSITION_HINT::UNDEFINED )
					{
						res = action;
						break;
					}
				}
			}
		}
		break;
		case SECTION_POSITION_HINT::UNDEFINED:
		case SECTION_POSITION_HINT::BOTTOM:
		{
			for ( QAction * action : actions() )
			{
				if ( action->isSeparator() && action->data().isValid() )
				{
					const SECTION_POSITION_HINT sectionPositionHint = SECTION_POSITION_HINT( action->data().toInt() );
					if ( sectionPositionHint == SECTION_POSITION_HINT::BOTTOM )
					{
						res = action;
						break;
					}
				}
			}
		}
		break;
		}

		return res;
	}

	void BaseContextualMenu::appendToSection( const QString & p_sectionName, QAction * const p_action )
	{
		QAction * section = _findSection( p_sectionName );

		if ( section == nullptr )
			section = addSection( p_sectionName );

		QAction * const before = _getNextSection( section );

		insertAction( before, p_action );
	}

	QAction * BaseContextualMenu::_findSection( const QString & p_sectionName ) const
	{
		for ( QAction * const action : actions() )
		{
			if ( action->isSeparator() && action->text() == p_sectionName )
			{
				return action;
			}
		}

		return nullptr;
	}

	QAction * BaseContextualMenu::_getNextSection( QAction * p_section ) const
	{
		QAction * res			= nullptr;
		bool	  startSeaching = false;

		for ( QAction * const action : actions() )
		{
			if ( !startSeaching )
			{
				if ( action == p_section )
				{
					startSeaching = true;
				}
			}
			else
			{
				if ( action->isSeparator() && action->data().isValid() )
				{
					res = action;
					break;
				}
			}
		}

		return res;
	}
} // namespace VTX::UI::QT::Core::ContextualMenu
