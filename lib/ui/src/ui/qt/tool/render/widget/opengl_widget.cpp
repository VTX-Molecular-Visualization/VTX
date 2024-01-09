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

		App::VTXApp::get().onPreRender().addCallback( this, [ this ]( float p_deltaTime ) { makeCurrent(); } );
		App::VTXApp::get().onPostRender().addCallback( this, [ this ]( float p_deltaTime ) { doneCurrent(); } );
	}

	OpenGLWidget::~OpenGLWidget() {}

	void OpenGLWidget::initializeGL()
	{
		assert( context()->isValid() );

		// TODO: setup callback instead of using singleton?

		// if ( _cbInitGL )
		//{
		//	_cbInitGL();
		// }

		makeCurrent();
		VTX::App::VTXApp::get().getRenderer().build( defaultFramebufferObject() );
		doneCurrent();
	}

	void OpenGLWidget::paintGL()
	{
		//_renderer->render();
	}

	void OpenGLWidget::resizeGL( int p_width, int p_height )
	{
		// if ( _cbResizeGL )
		//{
		//	_cbResizeGL( p_width, p_height );
		// }
	}
} // namespace VTX::UI::QT::Tool::Render::Widget
