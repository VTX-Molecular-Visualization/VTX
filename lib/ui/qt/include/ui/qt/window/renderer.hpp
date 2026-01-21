#ifndef __VTX_UI_QT_WINDOW_EVENT_CATCH_WINDOW__
#define __VTX_UI_QT_WINDOW_EVENT_CATCH_WINDOW__

#include "ui/qt/helper.hpp"
#include <QGuiApplication>
#include <QStyleHints>
#include <QWindow>
#include <app/input/input_manager.hpp>
#include <app/services.hpp>

namespace VTX::UI::QT::Window
{

	/**
	 * @brief A window that catches events and forwards them to the application's input manager.
	 */
	class Renderer : public QWindow
	{
		Q_OBJECT

	  public:
		using QWindow::QWindow;

	  signals:
		void clicked( const Qt::MouseButton, const QPoint );
		void dragged( const Qt::MouseButton );

	  protected:
		inline void keyPressEvent( QKeyEvent * const p_event ) override
		{
			App::INPUT().handleKeyboardEvent( Helper::qKeyEventToKeyEvent( *p_event ) );
		}

		inline void keyReleaseEvent( QKeyEvent * const p_event ) override
		{
			App::INPUT().handleKeyboardEvent( Helper::qKeyEventToKeyEvent( *p_event ) );
		}

		void mousePressEvent( QMouseEvent * p_event ) override
		{
			App::INPUT().handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );
			_pressedButton = p_event->button();
			_pressPos	   = p_event->position();
			_dragging	   = false;
		}

		void mouseMoveEvent( QMouseEvent * p_event ) override
		{
			App::INPUT().handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );
			if ( _pressedButton == Qt::NoButton )
			{
				return;
			}

			const int threshold = QGuiApplication::styleHints()->startDragDistance();
			if ( not _dragging && ( p_event->position() - _pressPos ).manhattanLength() >= threshold )
			{
				_dragging = true;
				emit dragged( _pressedButton );
			}
		}

		void mouseReleaseEvent( QMouseEvent * p_event ) override
		{
			App::INPUT().handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );
			if ( p_event->button() != _pressedButton )
			{
				return;
			}

			if ( not _dragging )
			{
				emit clicked( _pressedButton, _toDevicePixels( p_event->position() ) );
			}

			_pressedButton = Qt::NoButton;
			_dragging	   = false;
		}

		inline void mouseDoubleClickEvent( QMouseEvent * const p_event ) override
		{
			App::INPUT().handleMouseEvent( Helper::qMouseEventToMouseEvent( *p_event ) );
		}

		inline void wheelEvent( QWheelEvent * const p_event ) override
		{
			App::INPUT().handleMouseWheelEvent( Helper::qWheelEventToWheelEvent( *p_event ) );
		}

	  private:
		QPoint _toDevicePixels( const QPointF & p_logicalPos ) const
		{
			const qreal dpr = devicePixelRatio();
			const int	px	= int( p_logicalPos.x() * dpr );
			const int	py	= int( p_logicalPos.y() * dpr );

			return { px, py };
		}

	  private:
		Qt::MouseButton _pressedButton = Qt::NoButton;
		QPointF			_pressPos {};
		bool			_dragging = false;
	};

} // namespace VTX::UI::QT::Window

#endif
