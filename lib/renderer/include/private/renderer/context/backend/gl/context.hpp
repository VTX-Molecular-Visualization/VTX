#ifndef __VTX_RENDERER_CONTEXT_GL_CONTEXT__
#define __VTX_RENDERER_CONTEXT_GL_CONTEXT__

#include "include_opengl.hpp"
#include <cstdio>
#include <sstream>
#include <stdexcept>

// ============================================================
// WGL (Windows)
// ============================================================
#ifdef _WIN32

#include <GL/wglext.h>
#include <Windows.h>

namespace VTX::Renderer::Context::Backend::GL
{
	class WGLContextWrapper
	{
	  public:
		void init( const bool /*p_isWayland*/, const uintptr_t p_nativeWindow )
		{
			_hwnd = reinterpret_cast<HWND>( p_nativeWindow );
			_hdc  = GetDC( _hwnd );

			PIXELFORMATDESCRIPTOR pfd = {};
			pfd.nSize				  = sizeof( PIXELFORMATDESCRIPTOR );
			pfd.nVersion			  = 1;
			pfd.dwFlags				  = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.iPixelType			  = PFD_TYPE_RGBA;
			pfd.cColorBits			  = 32;
			pfd.cDepthBits			  = 24;
			pfd.cStencilBits		  = 8;
			pfd.iLayerType			  = PFD_MAIN_PLANE;

			const int pixelFormat = ChoosePixelFormat( _hdc, &pfd );
			if ( not pixelFormat )
			{
				throw std::runtime_error( "WGL: Failed to choose pixel format" );
			}
			if ( not SetPixelFormat( _hdc, pixelFormat, &pfd ) )
			{
				throw std::runtime_error( "WGL: Failed to set pixel format" );
			}

			// Temporary context to load WGL extensions.
			HGLRC tempContext = wglCreateContext( _hdc );
			if ( not tempContext )
			{
				throw std::runtime_error( "WGL: Failed to create temporary context" );
			}
			wglMakeCurrent( _hdc, tempContext );

			auto wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(
				wglGetProcAddress( "wglCreateContextAttribsARB" )
			);
			if ( not wglCreateContextAttribsARB )
			{
				throw std::runtime_error( "WGL: wglCreateContextAttribsARB not available" );
			}

			const int contextAttribs[] = { WGL_CONTEXT_MAJOR_VERSION_ARB,
										   VTX_OPENGL_MAJOR_VERSION,
										   WGL_CONTEXT_MINOR_VERSION_ARB,
										   VTX_OPENGL_MINOR_VERSION,

										   WGL_CONTEXT_PROFILE_MASK_ARB,
										   WGL_CONTEXT_CORE_PROFILE_BIT_ARB,

#ifdef _DEBUG
										   WGL_CONTEXT_FLAGS_ARB,
										   WGL_CONTEXT_DEBUG_BIT_ARB,
#endif

										   0 };

			_context = wglCreateContextAttribsARB( _hdc, nullptr, contextAttribs );
			wglMakeCurrent( nullptr, nullptr );
			wglDeleteContext( tempContext );

			if ( not _context )
			{
				throw std::runtime_error( "WGL: Failed to create context" );
			}

			makeCurrent();
			setSwapInterval( 1 );
		}

		static void * _wglLoader( const char * p_name )
		{
			void * p = reinterpret_cast<void *>( wglGetProcAddress( p_name ) );
			if ( not p )
			{
				static HMODULE opengl32 = LoadLibraryA( "opengl32.dll" );
				p						= reinterpret_cast<void *>( GetProcAddress( opengl32, p_name ) );
			}
			return p;
		}

		void * getProcAddress() { return reinterpret_cast<void *>( &_wglLoader ); }

		void makeCurrent()
		{
			if ( not wglMakeCurrent( _hdc, _context ) )
			{
				throw std::runtime_error( "WGL: makeCurrent failed" );
			}
		}

		void swapBuffers() const { SwapBuffers( _hdc ); }

		void setSwapInterval( const int p_interval )
		{
			auto wglSwapIntervalEXT
				= reinterpret_cast<PFNWGLSWAPINTERVALEXTPROC>( wglGetProcAddress( "wglSwapIntervalEXT" ) );
			if ( not wglSwapIntervalEXT )
			{
				throw std::runtime_error( "WGL: wglSwapIntervalEXT not available" );
			}
			wglSwapIntervalEXT( p_interval );
		}

		void destroy()
		{
			if ( not _hdc )
			{
				return;
			}

			wglMakeCurrent( nullptr, nullptr );

			if ( _context )
			{
				wglDeleteContext( _context );
				_context = nullptr;
			}

			ReleaseDC( _hwnd, _hdc );
			_hdc = nullptr;
		}

	  private:
		HWND  _hwnd	   = nullptr;
		HDC	  _hdc	   = nullptr;
		HGLRC _context = nullptr;
	};

	using GLContextWrapper = WGLContextWrapper;

} // namespace VTX::Renderer::Context::Backend::GL

// ============================================================
// EGL (Linux)
// ============================================================
#elif defined( __linux__ )

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <X11/Xlib.h>
#include <wayland-client.h>

namespace VTX::Renderer::Context::Backend::GL
{
	class EGLContextWrapper
	{
	  public:
		void init( const bool p_isWayland, const uintptr_t p_nativeWindow )
		{
			// Connect to the display matching Qt's platform.
			if ( p_isWayland )
			{
				_platform	   = Platform::Wayland;
				_nativeDisplay = wl_display_connect( nullptr );
				if ( not _nativeDisplay )
				{
					throw std::runtime_error( "EGL: Failed to connect to Wayland display" );
				}
				_display = eglGetPlatformDisplay( EGL_PLATFORM_WAYLAND_KHR, _nativeDisplay, nullptr );
			}
			else
			{
				// Use EGL_DEFAULT_DISPLAY to let Mesa pick the X11 connection internally,
				// avoiding the DRI3 path forced by EGL_PLATFORM_X11_KHR.
				_platform	   = Platform::X11;
				_nativeDisplay = nullptr;
				_display	   = eglGetDisplay( EGL_DEFAULT_DISPLAY );
			}

			if ( _display == EGL_NO_DISPLAY )
			{
				throw std::runtime_error( "EGL: Failed to get platform display" );
			}

			if ( not eglInitialize( _display, &_major, &_minor ) )
			{
				throw std::runtime_error( "EGL: Initialization failed" );
			}

			if ( not eglBindAPI( EGL_OPENGL_API ) )
			{
				throw std::runtime_error( "EGL: Failed to bind OpenGL API" );
			}

			const EGLint configAttribs[] = { EGL_SURFACE_TYPE,
											 EGL_WINDOW_BIT,
											 EGL_RENDERABLE_TYPE,
											 EGL_OPENGL_BIT,

											 EGL_RED_SIZE,
											 8,
											 EGL_GREEN_SIZE,
											 8,
											 EGL_BLUE_SIZE,
											 8,
											 EGL_ALPHA_SIZE,
											 8,

											 EGL_DEPTH_SIZE,
											 24,
											 EGL_STENCIL_SIZE,
											 8,

											 EGL_NONE };

			EGLint numConfigs = 0;

			if ( not eglChooseConfig( _display, configAttribs, &_config, 1, &numConfigs ) || numConfigs == 0 )
			{
				throw std::runtime_error( "EGL: Failed to choose config" );
			}

			_surface = eglCreateWindowSurface(
				_display, _config, reinterpret_cast<EGLNativeWindowType>( p_nativeWindow ), nullptr
			);

			if ( _surface == EGL_NO_SURFACE )
			{
				const EGLint	   err = eglGetError();
				std::ostringstream oss;
				oss << "EGL surface error: 0x" << std::hex << std::uppercase << err;
				throw std::runtime_error( oss.str() );
			}

			const EGLint contextAttribs[] = { EGL_CONTEXT_MAJOR_VERSION,
											  VTX_OPENGL_MAJOR_VERSION,
											  EGL_CONTEXT_MINOR_VERSION,
											  VTX_OPENGL_MINOR_VERSION,

											  EGL_CONTEXT_OPENGL_PROFILE_MASK,
											  EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,

#ifdef _DEBUG
											  EGL_CONTEXT_OPENGL_DEBUG,
											  EGL_TRUE,
#endif

											  EGL_NONE };

			_context = eglCreateContext( _display, _config, EGL_NO_CONTEXT, contextAttribs );

			if ( _context == EGL_NO_CONTEXT )
			{
				EGLint			   err = eglGetError();
				std::ostringstream oss;
				oss << "EGL context error: 0x" << std::hex << std::uppercase << err;
				throw std::runtime_error( oss.str() );
			}

			makeCurrent();
			setSwapInterval( 1 );
		}

		static void * _eglLoader( const char * p_name )
		{
			return reinterpret_cast<void *>( eglGetProcAddress( p_name ) );
		}

		void * getProcAddress() { return reinterpret_cast<void *>( &_eglLoader ); }

		void makeCurrent()
		{
			if ( not eglMakeCurrent( _display, _surface, _surface, _context ) )
			{
				throw std::runtime_error( "EGL: makeCurrent failed" );
			}
		}

		void swapBuffers() const
		{
			if ( not eglSwapBuffers( _display, _surface ) )
			{
				std::ostringstream oss;
				oss << "EGL: swapBuffers failed: 0x" << std::hex << std::uppercase << eglGetError();
				fprintf( stderr, "[EGL] %s\n", oss.str().c_str() );
			}
		}

		void setSwapInterval( const int p_interval )
		{
			if ( not eglSwapInterval( _display, p_interval ) )
			{
				throw std::runtime_error( "EGL: Failed to set swap interval" );
			}
		}

		void destroy()
		{
			if ( _display == EGL_NO_DISPLAY )
			{
				return;
			}

			eglMakeCurrent( _display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );

			if ( _context != EGL_NO_CONTEXT )
			{
				eglDestroyContext( _display, _context );
			}
			if ( _surface != EGL_NO_SURFACE )
			{
				eglDestroySurface( _display, _surface );
			}

			eglTerminate( _display );
			_display = EGL_NO_DISPLAY;

			if ( _platform == Platform::Wayland )
			{
				wl_display_disconnect( static_cast<wl_display *>( _nativeDisplay ) );
				_nativeDisplay = nullptr;
			}
		}

	  private:
		enum class Platform
		{
			X11,
			Wayland
		};

		EGLDisplay _display		  = EGL_NO_DISPLAY;
		EGLSurface _surface		  = EGL_NO_SURFACE;
		EGLContext _context		  = EGL_NO_CONTEXT;
		EGLConfig  _config		  = nullptr;
		Platform   _platform	  = Platform::X11;
		void *	   _nativeDisplay = nullptr;

		int _major = 0;
		int _minor = 0;
	};

	using GLContextWrapper = EGLContextWrapper;

} // namespace VTX::Renderer::Context::Backend::GL

#endif
#endif
