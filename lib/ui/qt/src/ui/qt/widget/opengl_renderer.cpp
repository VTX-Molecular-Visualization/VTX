#include "ui/qt/widget/opengl_renderer.hpp"
#include <QEvent>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QSurfaceFormat>
#include <app/services.hpp>
#include <renderer/renderer.hpp>
#include <util/logger.hpp>
#include <utility>

namespace VTX::UI::QT::Widget
{
	OpenGLRenderer::OpenGLRenderer( QWidget * const p_parent ) : QOpenGLWidget( p_parent )
	{
		setFormat( surfaceFormat() );
		setUpdateBehavior( QOpenGLWidget::PartialUpdate );

		connect(
			this,
			&QOpenGLWidget::frameSwapped,
			this,
			[ this ]()
			{
				if ( _glReady )
				{
					makeCurrent();
				}
			}
		);
	}

	OpenGLRenderer::~OpenGLRenderer() { destroyGLContext(); }

	QSurfaceFormat OpenGLRenderer::surfaceFormat()
	{
		QSurfaceFormat format;
		format.setRenderableType( QSurfaceFormat::OpenGL );
		format.setVersion(
			static_cast<int>( VTX::Renderer::Desc::OPENGL_MAJOR_VERSION ),
			static_cast<int>( VTX::Renderer::Desc::OPENGL_MINOR_VERSION )
		);
		format.setProfile( QSurfaceFormat::CoreProfile );
		format.setRedBufferSize( 8 );
		format.setGreenBufferSize( 8 );
		format.setBlueBufferSize( 8 );
		format.setAlphaBufferSize( 0 );
		format.setDepthBufferSize( 0 );
		format.setStencilBufferSize( 0 );
		format.setSwapBehavior( QSurfaceFormat::DoubleBuffer );
		format.setSwapInterval( 1 );
#ifdef _DEBUG
		format.setOption( QSurfaceFormat::DebugContext );
#endif

		return format;
	}

	bool OpenGLRenderer::isOpenGLSupported()
	{
		QOpenGLContext context;
		context.setFormat( surfaceFormat() );
		if ( not context.create() )
		{
			VTX_WARNING( "[Qt] Failed to create OpenGL context" );
			return false;
		}

		QOffscreenSurface surface;
		surface.setFormat( context.format() );
		surface.create();
		if ( not surface.isValid() || not context.makeCurrent( &surface ) )
		{
			VTX_WARNING( "[Qt] Failed to make OpenGL context current" );
			return false;
		}

		const QSurfaceFormat obtained = context.format();
		context.doneCurrent();

		const std::pair required { static_cast<int>( VTX::Renderer::Desc::OPENGL_MAJOR_VERSION ),
								   static_cast<int>( VTX::Renderer::Desc::OPENGL_MINOR_VERSION ) };
		const std::pair available { obtained.majorVersion(), obtained.minorVersion() };
		if ( available < required )
		{
			VTX_WARNING(
				"[Qt] OpenGL {}.{} required, {}.{} available",
				required.first,
				required.second,
				available.first,
				available.second
			);
			return false;
		}

		return true;
	}

	bool OpenGLRenderer::createGLContext()
	{
		if ( not isValid() )
		{
			VTX_WARNING( "[Qt] OpenGL context is not initialized" );
			return false;
		}

		makeCurrent();

		const QSurfaceFormat obtained = context()->format();
		VTX_INFO(
			"[Qt] OpenGL context {}.{} created (core={})",
			obtained.majorVersion(),
			obtained.minorVersion(),
			obtained.profile() == QSurfaceFormat::CoreProfile
		);

		_defaultFramebuffer = getGLDefaultFramebuffer();
		_glReady			= true;
		return true;
	}

	void OpenGLRenderer::destroyGLContext()
	{
		if ( not _glReady )
		{
			return;
		}

		_glReady = false;
		doneCurrent();
	}

	void OpenGLRenderer::swapGLBuffers()
	{
		if ( _glReady )
		{
			update();
		}
	}

	bool OpenGLRenderer::setGLSwapInterval( const int p_interval ) const
	{ return format().swapInterval() == p_interval; }

	void * OpenGLRenderer::getGLProcAddress( const char * const p_name ) const
	{
		if ( context() == nullptr )
		{
			return nullptr;
		}

		return reinterpret_cast<void *>( context()->getProcAddress( p_name ) );
	}

	uint32_t OpenGLRenderer::getGLDefaultFramebuffer() const
	{ return static_cast<uint32_t>( defaultFramebufferObject() ); }

	bool OpenGLRenderer::event( QEvent * const p_event )
	{
		const bool result = QOpenGLWidget::event( p_event );
		if ( p_event->type() == QEvent::DevicePixelRatioChange )
		{
			_syncDefaultFramebuffer();
		}
		return result;
	}

	void OpenGLRenderer::resizeGL( const int, const int )
	{
		_syncDefaultFramebuffer();
		App::RENDERER().setNeedUpdate();
	}

	void OpenGLRenderer::paintGL() { context()->functions()->glDisable( GL_BLEND ); }

	void OpenGLRenderer::_syncDefaultFramebuffer()
	{
		const uint32_t framebuffer = getGLDefaultFramebuffer();
		if ( not _glReady || framebuffer == _defaultFramebuffer )
		{
			return;
		}

		_defaultFramebuffer = framebuffer;
		App::RENDERER().setDefaultFramebuffer( framebuffer );
	}

} // namespace VTX::UI::QT::Widget
