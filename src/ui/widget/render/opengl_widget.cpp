#include "opengl_widget.hpp"
#include "object3d/camera.hpp"
#include "object3d/scene.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "util/opengl.hpp"
#include "vtx_app.hpp"
#include <QMainWindow>

namespace VTX::UI::Widget::Render
{
	OpenGLWidget::OpenGLWidget( QWidget * p_parent ) : QOpenGLWidget( p_parent ), Generic::BaseOpenGL( nullptr )
	{
		QSurfaceFormat format;
		format.setVersion( 4, 5 );
		format.setProfile( QSurfaceFormat::CoreProfile );
		format.setRenderableType( QSurfaceFormat::OpenGL );
		format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
		format.setSwapInterval( VTX_SETTING().activeVSync );
		QSurfaceFormat::setDefaultFormat( format );
	}

	OpenGLWidget::~OpenGLWidget()
	{
		makeCurrent();

		if ( _rendererGL != nullptr )
		{
			delete _rendererGL;
		}
#ifdef CUDA_DEFINED
		if ( _rendererRT != nullptr )
		{
			delete _rendererRT;
		}
#endif
#ifdef OPTIX_DEFINED
		if ( _rendererOptix != nullptr )
		{
			delete _rendererOptix;
		}
#endif

		doneCurrent();
	}

	void OpenGLWidget::initializeGL()
	{
		_gl = context()->versionFunctions<OpenGLFunctions>();
		_gl->initializeOpenGLFunctions();

#ifdef _DEBUG
		_gl->glEnable( GL_DEBUG_OUTPUT );
		_gl->glDebugMessageCallback( VTX::Util::OpenGL::debugMessageCallback, NULL );
#endif

		VTX_PROGRAM_MANAGER( _gl );
		switchRenderer( Setting::MODE_DEFAULT );
		getRenderer().init( Setting::WINDOW_WIDTH_DEFAULT, Setting::WINDOW_HEIGHT_DEFAULT );
		_timer.start();
	}

	void OpenGLWidget::paintGL()
	{
		getRenderer().renderFrame( VTXApp::get().getScene() );

		// TODO: write directly in defaultFramebufferObject()
		_gl->glBlitNamedFramebuffer( getRendererGL().getRenderedFBO(),
									 defaultFramebufferObject(),
									 0,
									 0,
									 size().width(),
									 size().height(),
									 0,
									 0,
									 size().width(),
									 size().height(),
									 GL_COLOR_BUFFER_BIT,
									 GL_NEAREST );

		_counter++;
		if ( _timer.elapsed() >= 1000.f )
		{
			uint fps	   = _counter / ( _timer.elapsed() / 1000.f );
			VTX_STAT().FPS = fps;
			_counter	   = 0;
			_timer.restart();
		}

		_painter.begin( this );
		_painter.setPen( Qt::white );
		_painter.drawText( 0, 10, QString::fromStdString( std::to_string( VTX_STAT().FPS ) ) );
		_painter.end();
	}

	void OpenGLWidget::resizeGL( int p_width, int p_height )
	{
		makeCurrent();
		VTXApp::get().getScene().getCamera().setScreenSize( p_width, p_height );
		getRenderer().resize( p_width, p_height );
		doneCurrent();
	}

	void OpenGLWidget::switchRenderer( const Renderer::MODE p_mode )
	{
		bool needInit = false;

		switch ( p_mode )
		{
		case Renderer::MODE::GL:
			assert( _gl != nullptr );
			if ( _rendererGL == nullptr )
			{
				_rendererGL = new Renderer::GL::GL( _gl );
				needInit	= true;
			}
			_renderer = _rendererGL;
			break;
#ifdef CUDA_DEFINED
		case Renderer::MODE::RT_CPU:
			if ( _rendererRT == nullptr )
			{
				_rendererRT = new Renderer::RayTracer();
				needInit	= true;
			}
			_renderer = _rendererRT;
			break;
#endif
#ifdef OPTIX_DEFINED
		case Renderer::MODE::RT_OPTIX:
			if ( _rendererOptix == nullptr )
			{
				_rendererOptix = new Renderer::Optix::OptixRayTracer();
				needInit	   = true;
			}
			_renderer = _rendererOptix;
			break;
#endif

		default: _renderer = nullptr;
		}

		if ( _renderer != nullptr )
		{
			/*
			if ( needInit )
			{
				_renderer->init( Setting::WINDOW_WIDTH_DEFAULT, Setting::WINDOW_HEIGHT_DEFAULT );
			}
			// Resize if needed.
			else if ( _renderer->getWidth() != getScene().getCamera().getScreenWidth() || _renderer->getHeight() !=
			getScene().getCamera().getScreenHeight() )
			{
				_renderer->resize( getScene().getCamera().getScreenWidth(), getScene().getCamera().getScreenHeight() );
			}
			*/
		}
	}

} // namespace VTX::UI::Widget::Render
