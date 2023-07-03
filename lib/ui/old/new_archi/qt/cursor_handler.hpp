#ifndef __VTX_UI_QT_CURSOR_HANDLER__
#define __VTX_UI_QT_CURSOR_HANDLER__

#include "ui/core/define.hpp"
#include <QCursor>
#include <QString>
#include <QWidget>
#include <set>
#include <vector>

namespace VTX::UI::QT
{
	class CursorHandler
	{
	  private:
		class AppliedCursor
		{
		  public:
			AppliedCursor( const Core::WidgetKey & p_cursor, const QString & p_caller ) :
				cursor( p_cursor ), caller( p_caller )
			{
			}

			const Core::WidgetKey cursor = Core::INVALID_WIDGET_KEY;
			QWidget *			  target = nullptr;
			const QString		  caller = "";
		};

	  public:
		CursorHandler();
		~CursorHandler();

		void referenceCursor( const Core::WidgetKey & p_key, const QString & p_spritePath );
		void referenceCursor( const Core::WidgetKey & p_key, const QCursor & p_cursor );

		void applyCursor( const Core::WidgetKey & p_key, QWidget * const p_target, const QString & p_caller );
		void removeCursor( const QString & p_caller );

	  private:
		std::map<Core::WidgetKey, QCursor> _cursorPixmaps = std::map<Core::WidgetKey, QCursor>();
		std::set<AppliedCursor *>		   _appliedCursor = std::set<AppliedCursor *>();

		void _setup();
	};

} // namespace VTX::UI::QT

#endif
