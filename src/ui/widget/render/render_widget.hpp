#ifndef __VTX_UI_WIDGET_RENDER__
#define __VTX_UI_WIDGET_RENDER__

#include "event/base_event_firerer_input.hpp"
#include "opengl_widget.hpp"
#include "overlay/base_overlay.hpp"
#include "overlay/visualization_quick_access.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QFocusEvent>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QWidget>
#include <map>

namespace VTX::UI::Widget::Render
{
	class BaseIntegratedWidget;

	class RenderWidget : public BaseManualWidget<QWidget>, public Event::BaseEventFirererInput
	{
		VTX_WIDGET

	  public:
		~RenderWidget();
		void localize() override;

		inline const OpenGLWidget & getOpenGLWidget() const { return *_openGLWidget; }
		inline OpenGLWidget &		getOpenGLWidget() { return *_openGLWidget; }

		void updateRender() const;

		void displayOverlay( const Overlay::OVERLAY & p_overlay, const Overlay::OVERLAY_ANCHOR & p_position );
		void hideOverlay( const Overlay::OVERLAY & p_overlay );

		void			 receiveEvent( const Event::VTXEvent & p_event ) override;
		const ID::VTX_ID getEventFirererId() const override { return ID::UI::Input::RENDER_WIDGET; }

	  protected:
		RenderWidget( QWidget * p_parent );

		void resizeEvent( QResizeEvent * p_event ) override;

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
		OpenGLWidget *									   _openGLWidget = nullptr;
		std::map<Overlay::OVERLAY, Overlay::BaseOverlay *> _overlays
			= std::map<Overlay::OVERLAY, Overlay::BaseOverlay *>();
		std::vector<BaseIntegratedWidget *> _integratedWidgets = std::vector<BaseIntegratedWidget *>();

		Overlay::BaseOverlay * _instantiateOverlay( const Overlay::OVERLAY & p_overlayType );

		// Shortcuts.
		void _onShortcutToggleCameraController();
		void _onShortcutResetCameraController();
		void _onShortcutAddViewpoint();
		void _onShortcutSnapshot();
		void _onShortcutChangeRenderMode();
	};
} // namespace VTX::UI::Widget::Render
#endif
