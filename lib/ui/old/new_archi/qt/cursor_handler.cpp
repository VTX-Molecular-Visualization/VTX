#include "ui/qt/cursor_handler.hpp"
#include "ui/qt/tool/keys.hpp"
#include <util/logger.hpp>

namespace VTX::UI::QT
{
	CursorHandler::CursorHandler() { _setup(); }
	void CursorHandler::_setup()
	{
		referenceCursor( QT::Tool::Cursor::DEFAULT, QCursor( Qt::CursorShape::ArrowCursor ) );

		// Move that to meassurement tools when implemented
		//
		// referenceCursor( QT::Tool::Cursor::MEASUREMENT_DISTANCE, ":/sprite/cursor/measurement_distance_cursor.png" );
		// referenceCursor( QT::Tool::Cursor::MEASUREMENT_DISTANCE_TO_CYCLE,
		//				 ":/sprite/cursor/measurement_distance_to_cycle_cursor.png" );
		// referenceCursor( QT::Tool::Cursor::MEASUREMENT_ANGLE, ":/sprite/cursor/measurement_angle_cursor.png" );
		// referenceCursor( QT::Tool::Cursor::MEASUREMENT_DIHEDRAL_ANGLE,
		//				 ":/sprite/cursor/measurement_dihedral_angle_cursor.png" );
	}

	CursorHandler::~CursorHandler()
	{
		for ( const AppliedCursor * const cursor : _appliedCursor )
		{
			delete cursor;
		}

		_appliedCursor.clear();
	}

	void CursorHandler::referenceCursor( const Core::WidgetKey & p_key, const QString & p_spritePath )
	{
		const QCursor cursor = QCursor( QPixmap( p_spritePath ), 1, 1 );
		referenceCursor( p_key, cursor );
	}
	void CursorHandler::referenceCursor( const Core::WidgetKey & p_key, const QCursor & p_cursor )
	{
		const std::map<Core::WidgetKey, QCursor>::const_iterator it = _cursorPixmaps.find( p_key );

		if ( it != _cursorPixmaps.end() )
		{
			VTX_WARNING( "Cursor with key {} already exists. It will be overrided.", p_key );
		}

		_cursorPixmaps[ p_key ] == p_cursor;
	}

	void CursorHandler::applyCursor( const Core::WidgetKey & p_key, QWidget * const p_target, const QString & p_caller )
	{
		const std::map<Core::WidgetKey, QCursor>::const_iterator it = _cursorPixmaps.find( p_key );

		if ( it == _cursorPixmaps.end() )
		{
			VTX_ERROR( "Cursor {} not referenced.", p_key );
			return;
		}

		const QCursor & cursor = it->second;
		p_target->setCursor( cursor );

		AppliedCursor * const appliedCursor = new AppliedCursor( p_key, p_caller );
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
		lastAppliedCursor->target->setCursor( _cursorPixmaps[ lastAppliedCursor->cursor ] );
	}

} // namespace VTX::UI::QT
