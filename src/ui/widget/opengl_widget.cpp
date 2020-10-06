#include "opengl_widget.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			OpenGLWidget::OpenGLWidget( QWidget * p_parent ) : QOpenGLWidget( p_parent )
			{
				format().setVersion( 4, 5 );
				format().setProfile( QSurfaceFormat::CompatibilityProfile );
				format().setRenderableType( QSurfaceFormat::OpenGL );

				switchRenderer( Setting::MODE_DEFAULT );
			}

			OpenGLWidget::~OpenGLWidget()
			{
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
			}

			void OpenGLWidget::initializeGL()
			{
				// VTXApp::get().getRenderer().init( Setting::WINDOW_WIDTH_DEFAULT, Setting::WINDOW_HEIGHT_DEFAULT );
			}

			void OpenGLWidget::paintGL() {}

			void OpenGLWidget::resizeGL( int p_width, int p_height )
			{
				// VTXApp::get().getScene().getCamera().setScreenSize( _width, _height );
				// VTXApp::get().getRenderer().resize( p_width, p_height );
			}

			void OpenGLWidget::switchRenderer( const Renderer::MODE p_mode )
			{
				bool needInit = false;

				switch ( p_mode )
				{
				case Renderer::MODE::GL:
					if ( _rendererGL == nullptr )
					{
						_rendererGL = new Renderer::GL();
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
					else if ( _renderer->getWidth() != getScene().getCamera().getScreenWidth() || _renderer->getHeight() != getScene().getCamera().getScreenHeight() )
					{
						_renderer->resize( getScene().getCamera().getScreenWidth(), getScene().getCamera().getScreenHeight() );
					}
					*/
				}
			}

		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
