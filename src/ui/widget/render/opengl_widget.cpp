#include "opengl_widget.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "object3d/camera.hpp"
#include "object3d/scene.hpp"
#include "renderer/gl/gl.hpp"
#include "renderer/gl/program_manager.hpp"
#include "renderer/rt/ray_tracer.hpp"
#include "ui/dialog.hpp"
#include "util/opengl.hpp"
#include "vtx_app.hpp"
#include <QDesktopWidget>
#include <QMainWindow>

namespace VTX::UI::Widget::Render
{
	OpenGLWidget::OpenGLWidget( QWidget * p_parent ) : BaseManualWidget<QOpenGLWidget>( p_parent )
	{
		QSurfaceFormat format;
		format.setVersion( OPENGL_MAJOR_VERSION, OPENGL_MINOR_VERSION );
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

		GLint glMaxTextureSize;
		GLint glMaxPatchVertices = 0;
		GLint glMaxTessGenLevel	 = 0;
		_gl->glGetIntegerv( GL_MAX_TEXTURE_SIZE, &glMaxTextureSize );
		_gl->glGetIntegerv( GL_MAX_PATCH_VERTICES, &glMaxPatchVertices );
		_gl->glGetIntegerv( GL_MAX_TESS_GEN_LEVEL, &glMaxTessGenLevel );

		VTX_SPEC().glVersion		  = std::string( (const char *)glVersion );
		VTX_SPEC().glslVersion		  = std::string( (const char *)glslVersion );
		VTX_SPEC().glVendor			  = std::string( (const char *)glVendor );
		VTX_SPEC().glRenderer		  = std::string( (const char *)glRenderer );
		VTX_SPEC().glMaxTextureSize	  = glMaxTextureSize;
		VTX_SPEC().glMaxPatchVertices = glMaxPatchVertices;
		VTX_SPEC().glMaxTessGenLevel  = glMaxTessGenLevel;

		VTX_INFO( "GL version: " + VTX_SPEC().glVersion );
		VTX_INFO( "GLSL version: " + VTX_SPEC().glslVersion );
		VTX_INFO( "GL device: " + VTX_SPEC().glVendor + " " + VTX_SPEC().glRenderer );

		VTX_DEBUG( "Max texture size : " + std::to_string( VTX_SPEC().glMaxTextureSize ) );
		VTX_DEBUG( "Max patch vertices: " + std::to_string( VTX_SPEC().glMaxPatchVertices ) );
		VTX_DEBUG( "Max tessellation gen level: " + std::to_string( VTX_SPEC().glMaxTessGenLevel ) );

#ifndef VTX_PRODUCTION
		_gl->glEnable( GL_DEBUG_OUTPUT );
		_gl->glDebugMessageCallback( VTX::Util::OpenGL::debugMessageCallback, NULL );
#endif

		VTX_PROGRAM_MANAGER();

		_frameTimer.start();
	}

	void OpenGLWidget::paintGL()
	{
		if ( _renderer == nullptr )
		{
			_switchRenderer( VTX_SETTING().mode );
		}

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
		_painter.begin( this );
		const QColor txtColor = VTX_RENDER_EFFECT().getBackgroundColor().brightness() < 0.5f ? Qt::GlobalColor::white
																							 : Qt::GlobalColor::black;
		_painter.setPen( txtColor );
		_painter.drawText( 0,
						   10,
						   QString::fromStdString( "FPS: " + std::to_string( VTX_STAT().FPS )
												   + " - draw calls: " + std::to_string( VTX_STAT().drawCalls ) ) );
		_painter.end();
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

		if ( _renderer != nullptr )
		{
			const float pixelRatio = VTXApp::get().getPixelRatio();
			getRenderer().resize( p_width * pixelRatio, p_height * pixelRatio, defaultFramebufferObject() );
		}

		doneCurrent();
	}

	void OpenGLWidget::_switchRenderer( const Renderer::MODE p_mode )
	{
		bool needInit = false;

		switch ( p_mode )
		{
		case Renderer::MODE::GL:
			assert( _gl != nullptr );
			if ( _rendererGL == nullptr )
			{
				_rendererGL = new Renderer::GL::GL();
				needInit	= true;
			}
			_renderer = _rendererGL;
			break;
		case Renderer::MODE::RT_CPU:
			assert( _gl != nullptr );
			if ( _rendererRT == nullptr )
			{
				_rendererRT = new Renderer::RayTracer();
				needInit	= true;
			}
			_renderer = _rendererRT;
			break;
		default: _renderer = nullptr;
		}

		if ( needInit )
		{
			const float pixelRatio = VTXApp::get().getPixelRatio();
			getRenderer().init( Setting::WINDOW_WIDTH_DEFAULT * pixelRatio,
								Setting::WINDOW_HEIGHT_DEFAULT * pixelRatio,
								defaultFramebufferObject() );
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

	const Vec2i OpenGLWidget::getPickedIds( const uint p_x, const uint p_y )
	{
		makeCurrent();
		return _renderer->getPickedIds( p_x, height() - p_y );
		doneCurrent();
	}

} // namespace VTX::UI::Widget::Render
