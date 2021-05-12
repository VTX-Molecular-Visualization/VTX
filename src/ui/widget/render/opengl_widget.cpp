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

		Renderer::GL::ProgramManager::get().dispose();

		doneCurrent();
	}

	void OpenGLWidget::initializeGL()
	{
		VTX_INFO( "Initializing OpenGL..." );
		_gl = context()->versionFunctions<OpenGLFunctions>();
		_gl->initializeOpenGLFunctions();

		const uchar * glVersion	  = _gl->glGetString( GL_VERSION );
		const uchar * glslVersion = _gl->glGetString( GL_SHADING_LANGUAGE_VERSION );
		const uchar * glVendor	  = _gl->glGetString( GL_VENDOR );
		const uchar * glRenderer  = _gl->glGetString( GL_RENDERER );

		VTX_INFO( "GL version: " + std::string( (const char *)glVersion ) );
		VTX_INFO( "GLSL version: " + std::string( (const char *)glslVersion ) );
		VTX_INFO( "GL device: " + std::string( (const char *)glVendor ) + " "
				  + std::string( (const char *)glRenderer ) );

#ifdef _DEBUG
		_gl->glEnable( GL_DEBUG_OUTPUT );
		_gl->glDebugMessageCallback( VTX::Util::OpenGL::debugMessageCallback, NULL );
#endif

		VTX_PROGRAM_MANAGER( _gl );
		switchRenderer( Setting::MODE_DEFAULT );
		getRenderer().init( Setting::WINDOW_WIDTH_DEFAULT, Setting::WINDOW_HEIGHT_DEFAULT, defaultFramebufferObject() );

		_frameTimer.start();
	}

	void OpenGLWidget::paintGL()
	{
		_frameCounter++;
		if ( _frameTimer.elapsed() >= 1000 )
		{
			VTX_STAT().FPS = _frameCounter / ( _frameTimer.elapsed() * 1e-3f );
			_frameCounter  = 0;
			_frameTimer.restart();
		}

		_timer.start();

		getRenderer().renderFrame( VTXApp::get().getScene() );

		VTX_STAT().renderTime = (float)_timer.nsecsElapsed() * 1e-6f;

		if ( DEV_MODE && _showCounter )
		{
			_painter.begin( this );
			_painter.setPen( Qt::white );
			_painter.drawText( 0, 10, QString::fromStdString( std::to_string( VTX_STAT().FPS ) ) );
			_painter.end();
		}
	}

	void OpenGLWidget::resizeGL( int p_width, int p_height )
	{
		makeCurrent();
		VTXApp::get().getScene().getCamera().setScreenSize( p_width, p_height );
		getRenderer().resize( p_width, p_height, defaultFramebufferObject() );
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
