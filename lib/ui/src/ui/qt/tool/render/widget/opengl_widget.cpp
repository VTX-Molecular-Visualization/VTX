#include "ui/qt/tool/render/widget/opengl_widget.hpp"
#include "ui/qt/application_qt.hpp"
#include <QOpenGLContext>
#include <app/application/scene.hpp>
#include <app/component/render/camera.hpp>
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

		App::RENDERER().build( defaultFramebufferObject() );
		App::VTXApp::get().onPostRender().addCallback( this, [ this ]( float p_deltaTime ) { update(); } );

		RendererQt renderer = QT_RENDERER();
		renderer.get().setMatrixView( App::SCENE().getCamera().getViewMatrix() );
		renderer.get().setMatrixProjection( App::SCENE().getCamera().getProjectionMatrix() );
		renderer.get().setCameraPosition( App::SCENE().getCamera().getTransform().getPosition() );
		renderer.get().setCameraClipInfos( App::SCENE().getCamera().getNear(), App::SCENE().getCamera().getFar() );
		renderer.get().setPerspective( App::SCENE().getCamera().isPerspective() );
	}

	void OpenGLWidget::paintGL() { VTX::App::VTXApp::get().getRenderer().render( 0 ); }

	void OpenGLWidget::resizeGL( int p_width, int p_height )
	{
		App::SCENE().getCamera().setScreenSize( width(), height() );

		RendererQt renderer = QT_RENDERER();
		renderer.get().resize( p_width, p_height );
		renderer.get().setOutput( defaultFramebufferObject() );
	}
} // namespace VTX::UI::QT::Tool::Render::Widget
