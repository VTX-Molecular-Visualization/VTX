#include "qt/opengl_widget.hpp"
#include <QOpenGLContext>
#include <app/application/renderer/renderer.hpp>
#include <app/application/scene.hpp>
#include <app/application/system/ecs_system.hpp>
#include <app/application/system/renderer.hpp>
#include <app/component/render/camera.hpp>
#include <app/component/render/proxy_camera.hpp>
#include <app/component/render/proxy_color_layout.hpp>
#include <app/component/render/proxy_molecule.hpp>
#include <app/component/scene/transform_component.hpp>
#include <app/vtx_app.hpp>
#include <renderer/proxy/representation.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT
{
	OpenGLWidget::OpenGLWidget( QWidget * p_parent ) : BaseWidget<OpenGLWidget, QOpenGLWidget>( p_parent )
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

		_makeCurrentCounter++;

		// We are in openGL context here => direct access to renderer
		VTX::Renderer::Facade & rendererFacade = App::RENDERER_SYSTEM().facade();

		// TODO: do not build there.
		rendererFacade.build( defaultFramebufferObject() );
		App::Component::Render::ProxyColorLayout & colorLayout
			= App::MAIN_REGISTRY().findComponent<App::Component::Render::ProxyColorLayout>();
		colorLayout.setup( rendererFacade );
		rendererFacade.setProxyColorLayout( colorLayout.getProxy().proxy() );
		static VTX::Renderer::Proxy::Representation			representation;
		std::vector<VTX::Renderer::Proxy::Representation *> representations { &representation };
		rendererFacade.addProxyRepresentations( representations );

		App::Component::Render::ProxyCamera & proxyCamera
			= App::MAIN_REGISTRY().getComponent<App::Component::Render::ProxyCamera>( App::SCENE().getCamera() );
		proxyCamera.setInRenderer( rendererFacade );

		App::RENDERER_SYSTEM().onGet += [ this ]()
		{
			if ( _makeCurrentCounter == 0 )
				makeCurrent();

			_makeCurrentCounter++;
		};
		App::RENDERER_SYSTEM().onRelease += [ this ]()
		{
			_makeCurrentCounter--;

			if ( _makeCurrentCounter == 0 )
				doneCurrent();
		};

		APP().onRender += [ this ]( const float p_deltaTime ) { update(); };
		_makeCurrentCounter--;
	}

	void OpenGLWidget::paintGL()
	{
		_makeCurrentCounter++;
		// We are in openGL context here => direct access to renderer
		App::RENDERER_SYSTEM().facade().render( 0.15f );
		_makeCurrentCounter--;
	}

	void OpenGLWidget::resizeGL( int p_width, int p_height )
	{
		_makeCurrentCounter++;
		App::SCENE().getCamera().setScreenSize( width(), height() );

		VTX::Renderer::Facade & rendererFacade = App::RENDERER_SYSTEM().facade();

		rendererFacade.resize( p_width, p_height );
		rendererFacade.setOutput( defaultFramebufferObject() );
		_makeCurrentCounter--;
	}
} // namespace VTX::UI::QT
