#ifndef __VTX_UI_QT_HELPER__
#define __VTX_UI_QT_HELPER__

#include <QKeyEvent>
#include <QMouseEvent>
#include <app/core/input/events.hpp>
#include <app/core/input/keys.hpp>
#include <unordered_map>

namespace VTX::UI::QT::Helper
{

	App::Core::Input::Key		 qKeyToKey( const Qt::Key & p_key );
	App::Core::Input::KeyEvent	 qKeyEventToKeyEvent( const QKeyEvent & p_event );
	App::Core::Input::MouseEvent qMouseEventToMouseEvent( const QMouseEvent & p_event );
	App::Core::Input::WheelEvent qWheelEventToWheelEvent( const QWheelEvent & p_event );

} // namespace VTX::UI::QT::Helper

#endif
