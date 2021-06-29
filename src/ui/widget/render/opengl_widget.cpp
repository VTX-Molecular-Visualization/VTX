#include "opengl_widget.hpp"
#include "object3d/camera.hpp"
#include "object3d/scene.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "ui/dialog.hpp"
#include "util/opengl.hpp"
#include "vtx_app.hpp"
#include <QDesktopWidget>
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
		format.setSwapInterval( VTX_SETTING().getVSync() );
		QSurfaceFormat::setDefaultFormat( format );
	}

	OpenGLWidget::~OpenGLWidget()
	{
		makeCurrent();

		if ( _rendererGL != nullptr )
		{
			delete _rendererGL;
		}

		Renderer::GL::ProgramManager::get().dispose();

		doneCurrent();
	}

	void OpenGLWidget::initializeGL()
	{
		VTX_INFO( "Initializing OpenGL..." );

		_gl = context()->versionFunctions<OpenGLFunctions>();

		if ( _gl == nullptr )
		{
			setUpdatesEnabled( false );
			Dialog::openGLInitializationFail();
			return;
		}

		_gl->initializeOpenGLFunctions();

		if ( !isValid() )
		{
			setUpdatesEnabled( false );
			Dialog::openGLInitializationFail();
			return;
		}

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

		float pixelRatio = VTXApp::get().desktop()->devicePixelRatioF();
		getRenderer().init( Setting::WINDOW_WIDTH_DEFAULT * pixelRatio,
							Setting::WINDOW_HEIGHT_DEFAULT * pixelRatio,
							defaultFramebufferObject() );

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

#ifndef VTX_PRODUCTION
		if ( _showCounter )
		{
			_painter.begin( this );
			_painter.setPen( Qt::white );
			_painter.drawText( 0,
							   10,
							   QString::fromStdString( "FPS: " + std::to_string( VTX_STAT().FPS )
													   + " - draw calls: " + std::to_string( VTX_STAT().drawCalls ) ) );
			_painter.end();
		}
#endif
	}

	void OpenGLWidget::resizeGL( int p_width, int p_height )
	{
		if ( _gl == nullptr ) // Avoid first resize if gl == nullptr.
		{
			return;
		}

		makeCurrent();
		VTXApp::get().getScene().getCamera().setScreenSize( p_width, p_height );
		float pixelRatio = VTXApp::get().desktop()->devicePixelRatioF();
		getRenderer().resize( p_width * pixelRatio, p_height * pixelRatio, defaultFramebufferObject() );
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
			}
			_renderer = _rendererGL;
			break;

		default: _renderer = nullptr;
		}
	}

	void OpenGLWidget::activeVSync( const bool p_active )
	{
		makeCurrent();
		/*
		QFunctionPointer func = context()->getProcAddress( "GLX_EXT_swap_control" );
		if ( func == nullptr )
		{
			VTX_DEBUG( "NULL" );
		}
		*/
		doneCurrent();
	}

} // namespace VTX::UI::Widget::Render
