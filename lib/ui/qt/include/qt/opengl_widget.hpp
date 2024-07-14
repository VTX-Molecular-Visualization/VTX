#ifndef __VTX_UI_QT_OPENGL_WIDGET__
#define __VTX_UI_QT_OPENGL_WIDGET__

#include "app/application/system/renderer.hpp"
#include "qt/base_widget.hpp"
#include <QOpenGLContext>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QPointer>
#include <QWindow>
#include <app/application/scene.hpp>
#include <app/component/render/camera.hpp>
#include <app/component/render/proxy_camera.hpp>
#include <app/component/render/proxy_color_layout.hpp>
#include <app/component/render/proxy_molecule.hpp>
#include <app/component/scene/transform_component.hpp>

namespace VTX
{
	namespace UI::QT
	{
		class OpenGLWidget : public QT::BaseWidget<OpenGLWidget, QWidget>
		{
		  public:
			OpenGLWidget( QWidget * p_parent )
			{
				QSurfaceFormat format;
				format.setVersion( 4, 5 );
				format.setProfile( QSurfaceFormat::CoreProfile );
				format.setRenderableType( QSurfaceFormat::OpenGL );
				format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
				format.setSwapInterval( 0 );
				format.setDepthBufferSize( 24 );
				format.setStencilBufferSize( 8 );
				QSurfaceFormat::setDefaultFormat( format );

				m_context = new QOpenGLContext();
				m_context->setFormat( format );
				m_context->create();

				m_surface = new QWindow();
				m_surface->setFormat( format );
				m_surface->setSurfaceType( QSurface::OpenGLSurface );
				m_surface->create();

				QScreen * screen = QGuiApplication::primaryScreen();

				m_context->setScreen( screen );
				m_surface->setScreen( screen );

				m_context->setShareContext( nullptr );

				if ( not m_context->isValid() )
				{
					throw std::runtime_error( "Failed to create OpenGL context" );
				}

				/*
				if ( not m_surface->isValid() )
				{
					throw std::runtime_error( "Failed to create OpenGL surface" );
				}
				*/
			}

			~OpenGLWidget() { m_context->doneCurrent(); }

			void init()
			{
				m_context->makeCurrent( m_surface );

				m_surface->resize( 1920, 1080 );
				m_surface->show();

				VTX::Renderer::Facade & rendererFacade = App::RENDERER_SYSTEM().facade();
				rendererFacade.build( m_context->defaultFramebufferObject() );
				App::Component::Render::ProxyColorLayout & colorLayout
					= App::MAIN_REGISTRY().findComponent<App::Component::Render::ProxyColorLayout>();
				colorLayout.setup( rendererFacade );
				rendererFacade.setProxyColorLayout( colorLayout.getProxy().proxy() );
				static VTX::Renderer::Proxy::Representation			representation;
				std::vector<VTX::Renderer::Proxy::Representation *> representations { &representation };
				rendererFacade.addProxyRepresentations( representations );

				App::Component::Render::ProxyCamera & proxyCamera
					= App::MAIN_REGISTRY().getComponent<App::Component::Render::ProxyCamera>( App::SCENE().getCamera()
					);
				proxyCamera.setInRenderer( rendererFacade );

				APP().onRender += [ this ]( const float p_deltaTime )
				{
					render();
					update();
				};
			}

			void render()
			{
				if ( !m_device )
					m_device = new QOpenGLPaintDevice();

				m_device->setSize( size() * m_surface->devicePixelRatio() );
				m_device->setDevicePixelRatio( m_surface->devicePixelRatio() );

				// QPainter painter( m_device );
				// render( &painter );

				App::RENDERER_SYSTEM().facade().render( 0.15f );

				m_context->swapBuffers( m_surface );
				m_context->makeCurrent( m_surface );
			}

		  private:
			QPointer<QOpenGLContext> m_context;
			QPointer<QWindow>		 m_surface;
			QOpenGLPaintDevice *	 m_device;
		};
	} // namespace UI::QT

} // namespace VTX

#endif
