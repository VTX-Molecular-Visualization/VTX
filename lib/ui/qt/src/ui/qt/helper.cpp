#include "ui/qt/helper.hpp"
#include <app/input/events.hpp>
#include <app/input/keys.hpp>
#include <util/color/rgba.hpp>

namespace VTX::UI::QT::Helper
{

	App::Input::Key qKeyToKey( const Qt::Key & p_key )
	{
		static std::unordered_map<Qt::Key, App::Input::Key> keyMap = {
			{ Qt::Key::Key_A, App::Input::Key::Key_A },
			{ Qt::Key::Key_B, App::Input::Key::Key_B },
			{ Qt::Key::Key_C, App::Input::Key::Key_C },
			{ Qt::Key::Key_D, App::Input::Key::Key_D },
			{ Qt::Key::Key_E, App::Input::Key::Key_E },
			{ Qt::Key::Key_F, App::Input::Key::Key_F },
			{ Qt::Key::Key_G, App::Input::Key::Key_G },
			{ Qt::Key::Key_H, App::Input::Key::Key_H },
			{ Qt::Key::Key_I, App::Input::Key::Key_I },
			{ Qt::Key::Key_J, App::Input::Key::Key_J },
			{ Qt::Key::Key_K, App::Input::Key::Key_K },
			{ Qt::Key::Key_L, App::Input::Key::Key_L },
			{ Qt::Key::Key_M, App::Input::Key::Key_M },
			{ Qt::Key::Key_N, App::Input::Key::Key_N },
			{ Qt::Key::Key_O, App::Input::Key::Key_O },
			{ Qt::Key::Key_P, App::Input::Key::Key_P },
			{ Qt::Key::Key_Q, App::Input::Key::Key_Q },
			{ Qt::Key::Key_R, App::Input::Key::Key_R },
			{ Qt::Key::Key_S, App::Input::Key::Key_S },
			{ Qt::Key::Key_T, App::Input::Key::Key_T },
			{ Qt::Key::Key_U, App::Input::Key::Key_U },
			{ Qt::Key::Key_V, App::Input::Key::Key_V },
			{ Qt::Key::Key_W, App::Input::Key::Key_W },
			{ Qt::Key::Key_X, App::Input::Key::Key_X },
			{ Qt::Key::Key_Y, App::Input::Key::Key_Y },
			{ Qt::Key::Key_Z, App::Input::Key::Key_Z },
			{ Qt::Key::Key_0, App::Input::Key::Key_0 },
			{ Qt::Key::Key_1, App::Input::Key::Key_1 },
			{ Qt::Key::Key_2, App::Input::Key::Key_2 },
			{ Qt::Key::Key_3, App::Input::Key::Key_3 },
			{ Qt::Key::Key_4, App::Input::Key::Key_4 },
			{ Qt::Key::Key_5, App::Input::Key::Key_5 },
			{ Qt::Key::Key_6, App::Input::Key::Key_6 },
			{ Qt::Key::Key_7, App::Input::Key::Key_7 },
			{ Qt::Key::Key_8, App::Input::Key::Key_8 },
			{ Qt::Key::Key_9, App::Input::Key::Key_9 },
			{ Qt::Key::Key_F1, App::Input::Key::Key_F1 },
			{ Qt::Key::Key_F2, App::Input::Key::Key_F2 },
			{ Qt::Key::Key_F3, App::Input::Key::Key_F3 },
			{ Qt::Key::Key_F4, App::Input::Key::Key_F4 },
			{ Qt::Key::Key_F5, App::Input::Key::Key_F5 },
			{ Qt::Key::Key_F6, App::Input::Key::Key_F6 },
			{ Qt::Key::Key_F7, App::Input::Key::Key_F7 },
			{ Qt::Key::Key_F8, App::Input::Key::Key_F8 },
			{ Qt::Key::Key_F9, App::Input::Key::Key_F9 },
			{ Qt::Key::Key_F10, App::Input::Key::Key_F10 },
			{ Qt::Key::Key_F11, App::Input::Key::Key_F11 },
			{ Qt::Key::Key_F12, App::Input::Key::Key_F12 },
			{ Qt::Key::Key_Control, App::Input::Key::Key_Control },
			{ Qt::Key::Key_Shift, App::Input::Key::Key_Shift },
			{ Qt::Key::Key_Alt, App::Input::Key::Key_Alt },
			{ Qt::Key::Key_AltGr, App::Input::Key::Key_AltGr },
		};

		if ( keyMap.contains( p_key ) )
		{
			return keyMap[ p_key ];
		}
		else
		{
			return App::Input::Key::Key_Unkown;
		}
	}

	App::Input::KeyEvent qKeyEventToKeyEvent( const QKeyEvent & p_event )
	{
		App::Input::KeyEvent event;
		event.isRepeating = p_event.isAutoRepeat();

		const Qt::Key key = static_cast<Qt::Key>( p_event.key() );
		event.key		  = qKeyToKey( key );

		switch ( p_event.type() )
		{
		case QEvent::KeyPress: event.type = App::Input::KeyEvent::Type::Press; break;
		case QEvent::KeyRelease: event.type = App::Input::KeyEvent::Type::Release; break;
		default: assert( false ); break;
		}

		return event;
	}

	App::Input::MouseEvent qMouseEventToMouseEvent( const QMouseEvent & p_event )
	{
		App::Input::MouseEvent event;
		event.position = { p_event.position().x(), p_event.position().y() };

		switch ( p_event.type() )
		{
		case QEvent::MouseButtonPress: event.type = App::Input::MouseEvent::Type::Press; break;
		case QEvent::MouseButtonRelease: event.type = App::Input::MouseEvent::Type::Release; break;
		case QEvent::MouseMove: event.type = App::Input::MouseEvent::Type::Move; break;
		case QEvent::MouseButtonDblClick: event.type = App::Input::MouseEvent::Type::DoubleClick; break;
		default: break;
		}

		switch ( p_event.button() )
		{
		case Qt::LeftButton: event.button = App::Input::MouseEvent::Button::Left; break;
		case Qt::RightButton: event.button = App::Input::MouseEvent::Button::Right; break;
		case Qt::MiddleButton: event.button = App::Input::MouseEvent::Button::Middle; break;
		default: break;
		}

		return event;
	}

	App::Input::WheelEvent qWheelEventToWheelEvent( const QWheelEvent & p_event )
	{
		App::Input::WheelEvent event;
		event.angleDelta = { p_event.angleDelta().x(), p_event.angleDelta().y() };

		switch ( p_event.type() )
		{
		case QEvent::Wheel: event.type = App::Input::WheelEvent::Type::Wheel; break;
		default: assert( false ); break;
		}

		return event;
	}

	QColor toQColor( const Util::Color::Rgba & p_color )
	{
		// return QColor( p_color.x * 255, p_color.y * 255, p_color.z * 255, p_color.w * 255 );
		return QColor( QString::fromStdString( p_color.toHexaString() ) );
	}

	Util::Color::Rgba fromQColor( const QColor & p_color )
	{
		return Util::Color::Rgba( p_color.redF(), p_color.greenF(), p_color.blueF(), p_color.alphaF() );
	}
} // namespace VTX::UI::QT::Helper
