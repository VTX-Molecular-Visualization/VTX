#ifndef __VTX_UI_QT_HELPER__
#define __VTX_UI_QT_HELPER__

#include <QColor>
#include <QKeyEvent>
#include <QMouseEvent>
#include <unordered_map>

namespace VTX::App::Core::Input
{
	enum struct Key;
	struct KeyEvent;
	struct MouseEvent;
	struct WheelEvent;
} // namespace VTX::App::Core::Input

namespace VTX::Util::Color
{
	class Rgba;
}

namespace VTX::UI::QT::Helper
{

	App::Core::Input::Key		 qKeyToKey( const Qt::Key & );
	App::Core::Input::KeyEvent	 qKeyEventToKeyEvent( const QKeyEvent & );
	App::Core::Input::MouseEvent qMouseEventToMouseEvent( const QMouseEvent & );
	App::Core::Input::WheelEvent qWheelEventToWheelEvent( const QWheelEvent & );

	QColor toQColor( const ::VTX::Util::Color::Rgba & );

} // namespace VTX::UI::QT::Helper

#endif
