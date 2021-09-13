#ifndef __VTX_UI_WIDGET_RENDER__
#define __VTX_UI_WIDGET_RENDER__

#include "opengl_widget.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include <QFocusEvent>
#include <QVBoxLayout>
#include <QWidget>

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

		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  protected:
		RenderWidget( QWidget * p_parent );

		void focusInEvent( QFocusEvent * p_event ) override;
		void focusOutEvent( QFocusEvent * p_event ) override;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		OpenGLWidget * _openGLWidget;
	};
} // namespace VTX::UI::Widget::Render
#endif
