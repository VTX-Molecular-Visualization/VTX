#include "ui/old_ui/ui/cursor_handler.hpp"
#include "ui/old_ui/vtx_app.hpp"

namespace VTX::UI
{
	CursorHandler::~CursorHandler()
	{
		for ( const AppliedCursor * const cursor : _appliedCursor )
		{
			delete cursor;
		}

		_appliedCursor.clear();
	}

	void CursorHandler::applyCursor( const Cursor & p_cursor, QWidget * const p_target, const QString & p_caller )
	{
		p_target->setCursor( _cursorPixmaps[ int( p_cursor ) ] );

		AppliedCursor * const appliedCursor = new AppliedCursor( p_cursor, p_caller );
		appliedCursor->target				= p_target;
		_appliedCursor.emplace( appliedCursor );
	}

	void CursorHandler::removeCursor( const QString & p_caller )
	{
		std::set<AppliedCursor *>::const_iterator it = _appliedCursor.cbegin();
		while ( it != _appliedCursor.cend() )
		{
			if ( ( *it )->caller == p_caller )
			{
				delete ( *it );
				_appliedCursor.erase( it );

				break;
			}
			it++;
		}

		const AppliedCursor * lastAppliedCursor = *( _appliedCursor.rbegin() );
		lastAppliedCursor->target->setCursor( _cursorPixmaps[ int( lastAppliedCursor->cursor ) ] );
	}

} // namespace VTX::UI
