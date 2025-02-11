#ifndef __VTX_UI_QT_WINDOW_EVENT_CATCH_WINDOW__
#define __VTX_UI_QT_WINDOW_EVENT_CATCH_WINDOW__

#include "ui/qt/helper.hpp"
#include <QWindow>
#include <app/core/input/input_manager.hpp>

namespace VTX::UI::QT::Window
{

	/**
	 * @brief A window that catches events and forwards them to the application's input manager.
	 */
	class EventCatchWindow : public QWindow
	{
	  public:
		inline void keyPressEvent( QKeyEvent * const p_event ) override
		{
			App::INPUT_MANAGER().handleKeyboardEvent( Helper::qKeyEventToKeyEvent( *p_event ) );
		}

		inline void keyReleaseEvent( QKeyEvent * const p_event ) override
		{
			App::INPUT_MANAGER().handleKeyboardEvent( Helper::qKeyEventToKeyEvent( *p_event ) );
		}

		inline void mousePressEvent( QMouseEvent * const p_event ) override
		{
			App::INPUT_MANAGER().handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );
		}

		inline void mouseReleaseEvent( QMouseEvent * const p_event ) override
		{
			App::INPUT_MANAGER().handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );
		}

		inline void mouseDoubleClickEvent( QMouseEvent * const p_event ) override
		{
			App::INPUT_MANAGER().handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );
		}

		inline void mouseMoveEvent( QMouseEvent * const p_event ) override
		{
			App::INPUT_MANAGER().handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );
		}

		inline void wheelEvent( QWheelEvent * const p_event ) override
		{
			App::INPUT_MANAGER().handleMouseWheelEvent( Helper::qWheelEventToWheelEvent( *p_event ) );
		}
	};

} // namespace VTX::UI::QT::Window

#endif
