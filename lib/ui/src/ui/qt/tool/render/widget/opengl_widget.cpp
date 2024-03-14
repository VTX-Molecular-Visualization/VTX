#include "ui/qt/tool/render/widget/opengl_widget.hpp"
#include "ui/qt/application_qt.hpp"
#include <QOpenGLContext>
#include <app/application/scene.hpp>
#include <app/component/render/camera.hpp>
#include <app/component/render/proxy_camera.hpp>
#include <app/component/render/proxy_molecule.hpp>
#include <app/component/scene/transform_component.hpp>
#include <app/vtx_app.hpp>
#include <core/chemdb/color.hpp>
#include <renderer/proxy/representations.hpp>
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
		App::RENDERER().setProxyColorLayout( VTX::Core::ChemDB::Color::COLOR_LAYOUT_JMOL );
		App::RENDERER().setProxyRepresentations( { VTX::Renderer::Proxy::Representation() } );

		App::Component::Render::ProxyCamera & proxyCamera
			= App::MAIN_REGISTRY().getComponent<App::Component::Render::ProxyCamera>( App::SCENE().getCamera() );
		proxyCamera.setInRenderer( App::RENDERER() );

		App::VTXApp::get().onPostRender += [ this ]( const float p_deltaTime ) { update(); };
	}

	void OpenGLWidget::paintGL() { VTX::App::VTXApp::get().getRenderer().render( 0.15f ); }

	void OpenGLWidget::resizeGL( int p_width, int p_height )
	{
		App::SCENE().getCamera().setScreenSize( width(), height() );

		RendererQt renderer = QT_RENDERER();
		renderer.get().resize( p_width, p_height );
		renderer.get().setOutput( defaultFramebufferObject() );
	}
} // namespace VTX::UI::QT::Tool::Render::Widget
