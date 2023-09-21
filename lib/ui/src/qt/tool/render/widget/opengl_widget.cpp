#include "ui/qt/tool/render/widget/opengl_widget.hpp"
#include <util/logger.hpp>

namespace VTX::UI::QT::Tool::Render::Widget
{
	OpenGLWidget::OpenGLWidget( QWidget * p_parent ) : BaseManualWidget<QOpenGLWidget>( p_parent ) {}

	OpenGLWidget::~OpenGLWidget() {}

	void OpenGLWidget::initializeGL() { VTX_DEBUG( "{}", "WidgetOpenGL::initializeGL()" ); }

	void OpenGLWidget::paintGL() {}

	void OpenGLWidget::resizeGL( int p_width, int p_height ) {}
} // namespace VTX::UI::QT::Tool::Render::Widget
