#include "ui/qt/tool/render/widget/opengl_widget.hpp"
#include <QOpenGLContext>
#include <app/vtx_app.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT::Tool::Render::Widget
{
	OpenGLWidget::OpenGLWidget( QWidget * p_parent ) : BaseManualWidget<QOpenGLWidget>( p_parent )
	{
		QSurfaceFormat format;
		format.setVersion( 4, 5 );
		format.setProfile( QSurfaceFormat::CoreProfile );
		format.setRenderableType( QSurfaceFormat::OpenGL );
		format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
		format.setSwapInterval( 0 );
		QSurfaceFormat::setDefaultFormat( format );
	}

	OpenGLWidget::~OpenGLWidget() {}

	void OpenGLWidget::initializeGL()
	{
		assert( context()->isValid() );

		VTX::App::VTXApp::get().getRenderer().build( defaultFramebufferObject() );
	}

	void OpenGLWidget::paintGL() { VTX::App::VTXApp::get().getRenderer().render( 0 ); }

	void OpenGLWidget::resizeGL( int p_width, int p_height )
	{
		VTX::App::VTXApp::get().getRenderer().resize( p_width, p_height );
	}
} // namespace VTX::UI::QT::Tool::Render::Widget
