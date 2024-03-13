#include "ui/qt/tool/render/widget/opengl_widget.hpp"
#include "ui/qt/application_qt.hpp"
#include <QOpenGLContext>
#include <app/application/scene.hpp>
#include <app/component/render/camera.hpp>
#include <app/component/render/proxy_camera.hpp>
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

		// TO_REMOVE : Force update camera position
		App::VTXApp::get().onUpdate().addCallback(
			this,
			[]( const float p_deltaTime )
			{
				const App::Component::Scene::Transform & transformComponent
					= App::MAIN_REGISTRY().getComponent<App::Component::Scene::Transform>( App::SCENE().getCamera() );

				const App::Component::Render::ProxyCamera & proxyComponent
					= App::MAIN_REGISTRY().getComponent<App::Component::Render::ProxyCamera>( App::SCENE().getCamera()
					);

				proxyComponent.getProxy().onCameraPosition( transformComponent.getPosition() );
			}
		);
	}

	OpenGLWidget::~OpenGLWidget() {}

	void OpenGLWidget::initializeGL()
	{
		assert( context()->isValid() );

		App::RENDERER().build( defaultFramebufferObject() );
		App::VTXApp::get().onPostRender().addCallback( this, [ this ]( float p_deltaTime ) { update(); } );
		App::RENDERER().setProxyColorLayout( VTX::Core::ChemDB::Color::COLOR_LAYOUT_JMOL );
		App::RENDERER().setProxyRepresentations( { VTX::Renderer::Proxy::Representation() } );
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
