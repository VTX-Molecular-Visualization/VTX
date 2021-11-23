#ifndef __VTX_UI_WIDGET_RENDER__
#define __VTX_UI_WIDGET_RENDER__

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
	class RenderWidget : public BaseManualWidget<QWidget>
	{
		VTX_WIDGET

	  public:
		~RenderWidget();
		void localize() override;

		inline const OpenGLWidget & getOpenGLWidget() const { return *_openGLWidget; }
		inline OpenGLWidget &		getOpenGLWidget() { return *_openGLWidget; }

		void displayOverlay( const Overlay::OVERLAY & p_overlay, const Overlay::OVERLAY_ANCHOR & p_position );
		void hideOverlay( const Overlay::OVERLAY & p_overlay );

		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  protected:
		RenderWidget( QWidget * p_parent );

		void focusInEvent( QFocusEvent * p_event ) override;
		void focusOutEvent( QFocusEvent * p_event ) override;

		void resizeEvent( QResizeEvent * p_event ) override;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		OpenGLWidget *									   _openGLWidget = nullptr;
		std::map<Overlay::OVERLAY, Overlay::BaseOverlay *> _overlays
			= std::map<Overlay::OVERLAY, Overlay::BaseOverlay *>();

		Overlay::BaseOverlay * _instantiateOverlay( const Overlay::OVERLAY & p_overlayType );
	};
} // namespace VTX::UI::Widget::Render
#endif
