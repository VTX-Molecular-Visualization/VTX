#ifndef __VTX_UI_WIDGET_RENDER__
#define __VTX_UI_WIDGET_RENDER__

#include "event/base_event_firerer_input.hpp"
#include "opengl_widget.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QFocusEvent>
#include <QVBoxLayout>
#include <QWidget>

namespace VTX::UI::Widget::Render
{
	class RenderWidget : public BaseManualWidget<QWidget>, public Event::BaseEventFirererInput
	{
		VTX_WIDGET

	  public:
		~RenderWidget();
		void localize() override;

		inline const OpenGLWidget & getOpenGLWidget() const { return *_openGLWidget; }
		inline OpenGLWidget &		getOpenGLWidget() { return *_openGLWidget; }

		void			 receiveEvent( const Event::VTXEvent & p_event ) override;
		const ID::VTX_ID getEventFirererId() const override { return ID::UI::Input::RENDER_WIDGET; }

	  protected:
		RenderWidget( QWidget * p_parent );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void mouseMoveEvent( QMouseEvent * const p_event ) override { _fireEventInput( p_event ); }
		void mousePressEvent( QMouseEvent * const p_event ) override { _fireEventInput( p_event ); }
		void mouseReleaseEvent( QMouseEvent * const p_event ) override { _fireEventInput( p_event ); }
		void mouseDoubleClickEvent( QMouseEvent * const p_event ) override { _fireEventInput( p_event ); }
		void keyPressEvent( QKeyEvent * const p_event ) override { _fireEventInput( p_event ); }
		void keyReleaseEvent( QKeyEvent * const p_event ) override { _fireEventInput( p_event ); }
		void wheelEvent( QWheelEvent * const p_event ) override { _fireEventInput( p_event ); }

	  private:
		OpenGLWidget * _openGLWidget = nullptr;

		// Shortcuts.
		void _onShortcutToggleCameraController();
		void _onShortcutResetCameraController();
		void _onShortcutAddViewpoint();
		void _onShortcutSnapshot();
		void _onShortcutChangeRenderMode();
	};
} // namespace VTX::UI::Widget::Render
#endif
