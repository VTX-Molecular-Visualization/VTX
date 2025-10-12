#ifndef __VTX_UI_QT_HELPER__
#define __VTX_UI_QT_HELPER__

#include <QColor>
#include <QKeyEvent>
#include <QMouseEvent>
#include <unordered_map>
#include <util/color/rgba.hpp>

namespace VTX::App::Input
{
	enum struct Key;
	struct KeyEvent;
	struct MouseEvent;
	struct WheelEvent;
} // namespace VTX::App::Input

namespace VTX::UI::QT::Helper
{

	App::Input::Key		 qKeyToKey( const Qt::Key & );
	App::Input::KeyEvent	 qKeyEventToKeyEvent( const QKeyEvent & );
	App::Input::MouseEvent qMouseEventToMouseEvent( const QMouseEvent & );
	App::Input::WheelEvent qWheelEventToWheelEvent( const QWheelEvent & );

	VTX::Util::Color::Rgba fromQColor( const QColor & );
	QColor				   toQColor( const VTX::Util::Color::Rgba & );

} // namespace VTX::UI::QT::Helper

#endif
