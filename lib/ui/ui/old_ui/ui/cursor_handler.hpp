#ifndef __VTX_UI_CURSOR_HANDLER__
#define __VTX_UI_CURSOR_HANDLER__

#include "old_ui/ui/widget/contextual_menu/contextual_menu_template.hpp"
#include <QCursor>
#include <QWidget>
#include <set>
#include <vector>

namespace VTX::UI
{
	class CursorHandler
	{
	  public:
		enum class Cursor : int
		{
			DEFAULT,
			MEASUREMENT_DISTANCE,
			MEASUREMENT_DISTANCE_TO_CYCLE,
			MEASUREMENT_ANGLE,
			MEASUREMENT_DIHEDRAL_ANGLE,

			COUNT
		};

	  private:
		class AppliedCursor
		{
		  public:
			AppliedCursor( const Cursor & p_cursor, const QString & p_caller ) : cursor( p_cursor ), caller( p_caller )
			{
			}

			const Cursor  cursor = Cursor::DEFAULT;
			QWidget *	  target = nullptr;
			const QString caller = "";
		};

	  public:
		CursorHandler()
		{
			_cursorPixmaps.resize( int( Cursor::COUNT ) );

			_cursorPixmaps[ int( Cursor::DEFAULT ) ] = QCursor( Qt::CursorShape::ArrowCursor );
			_cursorPixmaps[ int( Cursor::MEASUREMENT_DISTANCE ) ]
				= QCursor( QPixmap( ":/sprite/cursor/measurement_distance_cursor.png" ), 1, 1 );
			_cursorPixmaps[ int( Cursor::MEASUREMENT_DISTANCE_TO_CYCLE ) ]
				= QCursor( QPixmap( ":/sprite/cursor/measurement_distance_to_cycle_cursor.png" ), 1, 1 );
			_cursorPixmaps[ int( Cursor::MEASUREMENT_ANGLE ) ]
				= QCursor( QPixmap( ":/sprite/cursor/measurement_angle_cursor.png" ), 1, 1 );
			_cursorPixmaps[ int( Cursor::MEASUREMENT_DIHEDRAL_ANGLE ) ]
				= QCursor( QPixmap( ":/sprite/cursor/measurement_dihedral_angle_cursor.png" ), 1, 1 );
		}
		~CursorHandler();

		void applyCursor( const Cursor & p_cursor, QWidget * const p_target, const QString & p_caller );
		void removeCursor( const QString & p_caller );

	  private:
		std::vector<QCursor>	  _cursorPixmaps = std::vector<QCursor>();
		std::set<AppliedCursor *> _appliedCursor = std::set<AppliedCursor *>();
	};

} // namespace VTX::UI

#endif
