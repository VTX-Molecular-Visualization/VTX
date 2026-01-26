#ifndef __VTX_UI_QT_WINDOW_RENDERER__
#define __VTX_UI_QT_WINDOW_RENDERER__

#include <QWindow>

namespace VTX::UI::QT::Window
{

	/**
	 * @brief A window that catches events and forwards them to the application's input manager.
	 */
	class Renderer : public QWindow
	{
		Q_OBJECT

	  public:
	  signals:
		void clicked( const Qt::MouseButton, const QPoint );
		void dragged( const Qt::MouseButton );

	  protected:
		void keyPressEvent( QKeyEvent * const ) override;
		void keyReleaseEvent( QKeyEvent * const ) override;
		void mousePressEvent( QMouseEvent * ) override;
		void mouseMoveEvent( QMouseEvent * ) override;
		void mouseReleaseEvent( QMouseEvent * ) override;
		void mouseDoubleClickEvent( QMouseEvent * const ) override;
		void wheelEvent( QWheelEvent * const ) override;

	  private:
		Qt::MouseButton _pressedButton = Qt::NoButton;
		QPointF			_pressPos {};
		bool			_dragging = false;

		QPoint _toDevicePixels( const QPointF & ) const;
	};

} // namespace VTX::UI::QT::Window

#endif
