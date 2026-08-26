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
#include <renderer/descriptors.hpp>
#include <util/logger.hpp>

namespace VTX::Renderer::Context::Backend::GL
{
	class WGLContextWrapper
	{
	  public:
		WGLContextWrapper() = default;

		~WGLContextWrapper() { destroy(); }

		void init( const Desc::NativeContextInfo & p_contextInfo, const uint32_t, const uint32_t )
		{
			VTX_TRACE( "[WGL] Creating context for surface {}", p_contextInfo.surface );

			_hwnd = reinterpret_cast<HWND>( p_contextInfo.surface );
			_hdc  = GetDC( _hwnd );
			if ( not _hdc )
			{
				throw std::runtime_error( "WGL: Failed to get device context" );
			}

			PIXELFORMATDESCRIPTOR pfd = _defaultPixelFormatDescriptor();

			// Bootstrap WGL extensions from a dummy window/context before configuring the real surface.
			const HMODULE instance	= GetModuleHandleA( nullptr );
			const char *  className = "VTX_WGLBootstrap";
			WNDCLASSA	  wc		= {};
			wc.style				= CS_OWNDC;
			wc.lpfnWndProc			= DefWindowProcA;
			wc.hInstance			= instance;
			wc.lpszClassName		= className;

			if ( not RegisterClassA( &wc ) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS )
			{
				throw std::runtime_error( "WGL: Failed to register bootstrap window class" );
			}

			HWND bootstrapWindow = CreateWindowA(
				className, "VTX WGL Bootstrap", WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, nullptr, nullptr, instance, nullptr
			);
			if ( not bootstrapWindow )
			{
				throw std::runtime_error( "WGL: Failed to create bootstrap window" );
			}

			HDC bootstrapDc = GetDC( bootstrapWindow );
			if ( not bootstrapDc )
			{
				DestroyWindow( bootstrapWindow );
				throw std::runtime_error( "WGL: Failed to get bootstrap device context" );
			}

			const PIXELFORMATDESCRIPTOR bootstrapPfd	= _defaultPixelFormatDescriptor();
			const int					bootstrapFormat = ChoosePixelFormat( bootstrapDc, &bootstrapPfd );
			if ( not bootstrapFormat || not SetPixelFormat( bootstrapDc, bootstrapFormat, &bootstrapPfd ) )
			{
				ReleaseDC( bootstrapWindow, bootstrapDc );
				DestroyWindow( bootstrapWindow );
				throw std::runtime_error( "WGL: Failed to setup bootstrap pixel format" );
			}

			HGLRC tempContext = wglCreateContext( bootstrapDc );
			if ( not tempContext )
			{
				ReleaseDC( bootstrapWindow, bootstrapDc );
				DestroyWindow( bootstrapWindow );
				throw std::runtime_error( "WGL: Failed to create temporary context" );
			}
			wglMakeCurrent( bootstrapDc, tempContext );

			auto wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(
				wglGetProcAddress( "wglCreateContextAttribsARB" )
			);
			if ( not wglCreateContextAttribsARB )
			{
				wglMakeCurrent( nullptr, nullptr );
				wglDeleteContext( tempContext );
				ReleaseDC( bootstrapWindow, bootstrapDc );
				DestroyWindow( bootstrapWindow );
				throw std::runtime_error( "WGL: wglCreateContextAttribsARB not available" );
			}

			auto wglChoosePixelFormatARB
				= reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>( wglGetProcAddress( "wglChoosePixelFormatARB" ) );
			VTX_TRACE(
				"[WGL] Extensions loaded: create_context={}, choose_pixel_format={}",
				wglCreateContextAttribsARB != nullptr,
				wglChoosePixelFormatARB != nullptr
			);

			wglMakeCurrent( nullptr, nullptr );
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

			int pixelFormat = 0;
			try
			{
				pixelFormat = _choosePixelFormat( wglChoosePixelFormatARB, pfd );
			}
			catch ( const std::exception & )
			{
				wglDeleteContext( tempContext );
				ReleaseDC( bootstrapWindow, bootstrapDc );
				DestroyWindow( bootstrapWindow );
				throw;
			}
			if ( not SetPixelFormat( _hdc, pixelFormat, &pfd ) )
			{
				wglDeleteContext( tempContext );
				ReleaseDC( bootstrapWindow, bootstrapDc );
				DestroyWindow( bootstrapWindow );
				throw std::runtime_error( "WGL: Failed to set pixel format" );
			}

			_context = wglCreateContextAttribsARB( _hdc, nullptr, contextAttribs );
			wglDeleteContext( tempContext );
			ReleaseDC( bootstrapWindow, bootstrapDc );
			DestroyWindow( bootstrapWindow );

			if ( not _context )
			{
				throw std::runtime_error( "WGL: Failed to create context" );
			}

			VTX_TRACE( "[WGL] Core profile context {}.{} created", VTX_OPENGL_MAJOR_VERSION, VTX_OPENGL_MINOR_VERSION );
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

		void makeCurrent() const
		{
			if ( not wglMakeCurrent( _hdc, _context ) )
			{
				throw std::runtime_error( "WGL: makeCurrent failed" );
			}
		}

		void swapBuffers() const { SwapBuffers( _hdc ); }

		bool isDefaultFramebufferSrgb() const { return _defaultFramebufferSrgb; }

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

		void resize( const uint32_t, const uint32_t ) {}

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
		static PIXELFORMATDESCRIPTOR _defaultPixelFormatDescriptor()
		{
			PIXELFORMATDESCRIPTOR pfd = {};
			pfd.nSize				  = sizeof( PIXELFORMATDESCRIPTOR );
			pfd.nVersion			  = 1;
			pfd.dwFlags				  = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
			pfd.iPixelType			  = PFD_TYPE_RGBA;
			pfd.cColorBits			  = 32;
			pfd.cDepthBits			  = 24;
			pfd.cStencilBits		  = 8;
			pfd.iLayerType			  = PFD_MAIN_PLANE;
			return pfd;
		}

		int _choosePixelFormat(
			const PFNWGLCHOOSEPIXELFORMATARBPROC p_choosePixelFormat,
			PIXELFORMATDESCRIPTOR &				 p_pfd
		)
		{
			int pixelFormat = 0;

			if ( p_choosePixelFormat )
			{
				int pixelAttribs[ 17 ] = { WGL_DRAW_TO_WINDOW_ARB, 1,  WGL_SUPPORT_OPENGL_ARB, 1,
										   WGL_DOUBLE_BUFFER_ARB,  1,  WGL_PIXEL_TYPE_ARB,	   WGL_TYPE_RGBA_ARB,
										   WGL_COLOR_BITS_ARB,	   32, WGL_DEPTH_BITS_ARB,	   24,
										   WGL_STENCIL_BITS_ARB,   8 };

				const auto chooseFormat = [ & ]( const int * const p_attributes )
				{
					UINT numFormats = 0;
					pixelFormat		= 0;
					return p_choosePixelFormat( _hdc, p_attributes, nullptr, 1, &pixelFormat, &numFormats )
						   && numFormats > 0;
				};

				pixelAttribs[ 14 ]		= WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB;
				pixelAttribs[ 15 ]		= 1;
				const bool selectedSrgb = chooseFormat( pixelAttribs );
				pixelAttribs[ 14 ]		= 0;
				pixelAttribs[ 15 ]		= 0;
				if ( not selectedSrgb )
				{
					VTX_WARNING( "[WGL] No sRGB pixel format available, falling back to a linear format" );
				}

				if ( selectedSrgb || chooseFormat( pixelAttribs ) )
				{
					PIXELFORMATDESCRIPTOR chosen = {};
					if ( DescribePixelFormat( _hdc, pixelFormat, sizeof( chosen ), &chosen ) != 0 )
					{
						p_pfd					= chosen;
						_defaultFramebufferSrgb = selectedSrgb;
						if ( selectedSrgb )
						{
							VTX_INFO( "[WGL] Using sRGB pixel format {}", pixelFormat );
						}
						VTX_TRACE(
							"[WGL] Using ARB pixel format {} (color={}, depth={}, stencil={}, double_buffer={}, "
							"srgb={})",
							pixelFormat,
							int( p_pfd.cColorBits ),
							int( p_pfd.cDepthBits ),
							int( p_pfd.cStencilBits ),
							( p_pfd.dwFlags & PFD_DOUBLEBUFFER ) != 0,
							selectedSrgb
						);
						return pixelFormat;
					}
				}
				VTX_TRACE(
					"[WGL] wglChoosePixelFormatARB available but no suitable format was selected, falling back"
				);
			}

			pixelFormat = ChoosePixelFormat( _hdc, &p_pfd );
			if ( not pixelFormat )
			{
				throw std::runtime_error( "WGL: Failed to choose pixel format" );
			}
			VTX_TRACE( "[WGL] Falling back to ChoosePixelFormat -> {}", pixelFormat );

			return pixelFormat;
		}

		HWND  _hwnd					  = nullptr;
		HDC	  _hdc					  = nullptr;
		HGLRC _context				  = nullptr;
		bool  _defaultFramebufferSrgb = false;
	};

	using GLContextWrapper = WGLContextWrapper;

} // namespace VTX::Renderer::Context::Backend::GL

// ============================================================
// CGL/NSOpenGL (macOS)
// ============================================================
#elif defined( __APPLE__ )

#include <renderer/descriptors.hpp>

namespace VTX::Renderer::Context::Backend::GL
{
	class MacOSGLContextWrapper
	{
	  public:
		MacOSGLContextWrapper() = default;

		~MacOSGLContextWrapper() { destroy(); }

		void init( const Desc::NativeContextInfo &, const uint32_t, const uint32_t )
		{
			throw std::runtime_error( "macOS OpenGL context creation is not implemented yet" );
		}

		void * getProcAddress() { return nullptr; }

		void makeCurrent() const { throw std::runtime_error( "macOS OpenGL context is not initialized" ); }

		void swapBuffers() const {}

		bool isDefaultFramebufferSrgb() const { return false; }

		void setSwapInterval( int ) {}

		void resize( const uint32_t, const uint32_t ) {}

		void destroy() {}
	};

	using GLContextWrapper = MacOSGLContextWrapper;

} // namespace VTX::Renderer::Context::Backend::GL

// ============================================================
// EGL (Linux)
// ============================================================
#elif defined( __linux__ )

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <renderer/descriptors.hpp>
#include <util/logger.hpp>
#include <wayland-egl.h>

namespace VTX::Renderer::Context::Backend::GL
{
	class EGLContextWrapper
	{
	  public:
		EGLContextWrapper() = default;

		~EGLContextWrapper() { destroy(); }

		void init( const Desc::NativeContextInfo & p_contextInfo, const uint32_t p_width, const uint32_t p_height )
		{
			VTX_TRACE(
				"[EGL] Creating context for surface {} with display {} and platform {}",
				p_contextInfo.surface,
				p_contextInfo.display,
				int( p_contextInfo.plateform )
			);

			_nativeDisplay = reinterpret_cast<void *>( p_contextInfo.display );
			switch ( p_contextInfo.plateform )
			{
			case Desc::E_NATIVE_PLATEFORM::WAYLAND:
			{
				_platform = Platform::Wayland;
				if ( not _nativeDisplay )
				{
					throw std::runtime_error( "EGL: Wayland platform requires a native display" );
				}
				VTX_TRACE( "[EGL] Using Wayland platform display" );
				VTX_TRACE( "[EGL] Calling eglGetPlatformDisplay(WAYLAND)" );
				_display = eglGetPlatformDisplay( EGL_PLATFORM_WAYLAND_KHR, _nativeDisplay, nullptr );
				VTX_TRACE( "[EGL] eglGetPlatformDisplay(WAYLAND) -> {}", reinterpret_cast<uintptr_t>( _display ) );
				break;
			}
			case Desc::E_NATIVE_PLATEFORM::X11:
			{
				_platform = Platform::X11;
				if ( _nativeDisplay )
				{
					VTX_TRACE( "[EGL] Using X11 platform display" );
					VTX_TRACE( "[EGL] Calling eglGetPlatformDisplay(X11)" );
					_display = eglGetPlatformDisplay( EGL_PLATFORM_X11_KHR, _nativeDisplay, nullptr );
					VTX_TRACE( "[EGL] eglGetPlatformDisplay(X11) -> {}", reinterpret_cast<uintptr_t>( _display ) );
				}
				break;
			}
			case Desc::E_NATIVE_PLATEFORM::UNKNOWN:
			default:
			{
				VTX_TRACE( "[EGL] No explicit native platform provided, using EGL default display" );
				_platform	   = Platform::Default;
				_nativeDisplay = nullptr;
				VTX_TRACE( "[EGL] Calling eglGetDisplay(EGL_DEFAULT_DISPLAY)" );
				_display = eglGetDisplay( EGL_DEFAULT_DISPLAY );
				VTX_TRACE( "[EGL] eglGetDisplay(EGL_DEFAULT_DISPLAY) -> {}", reinterpret_cast<uintptr_t>( _display ) );
				break;
			}
			}

			if ( _display == EGL_NO_DISPLAY && _platform == Platform::X11 )
			{
				VTX_TRACE( "[EGL] X11 platform display unavailable, using EGL default display" );
				_nativeDisplay = nullptr;
				_platform	   = Platform::Default;
				VTX_TRACE( "[EGL] Calling eglGetDisplay(EGL_DEFAULT_DISPLAY) fallback" );
				_display = eglGetDisplay( EGL_DEFAULT_DISPLAY );
				VTX_TRACE(
					"[EGL] eglGetDisplay(EGL_DEFAULT_DISPLAY) fallback -> {}", reinterpret_cast<uintptr_t>( _display )
				);
			}

			if ( _display == EGL_NO_DISPLAY )
			{
				throw std::runtime_error( "EGL: Failed to get platform display" );
			}

			VTX_TRACE( "[EGL] Calling eglInitialize" );
			if ( not eglInitialize( _display, &_major, &_minor ) )
			{
				throw std::runtime_error( "EGL: Initialization failed" );
			}
			VTX_TRACE( "[EGL] Initialized EGL {}.{}", _major, _minor );

			VTX_TRACE( "[EGL] Calling eglBindAPI(EGL_OPENGL_API)" );
			if ( not eglBindAPI( EGL_OPENGL_API ) )
			{
				throw std::runtime_error( "EGL: Failed to bind OpenGL API" );
			}
			VTX_TRACE( "[EGL] eglBindAPI(EGL_OPENGL_API) succeeded" );

			EGLint nativeVisualId = EGL_DONT_CARE;
			if ( _platform == Platform::X11 )
			{
				XWindowAttributes windowAttributes;
				if ( not XGetWindowAttributes(
						 reinterpret_cast<Display *>( _nativeDisplay ),
						 static_cast<Window>( p_contextInfo.surface ),
						 &windowAttributes
					 ) )
				{
					throw std::runtime_error( "EGL: Failed to query X11 window attributes" );
				}
				nativeVisualId = static_cast<EGLint>( XVisualIDFromVisual( windowAttributes.visual ) );
				VTX_TRACE( "[EGL] X11 native visual ID: {}", nativeVisualId );
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
											 EGL_NATIVE_VISUAL_ID,
											 nativeVisualId,

											 EGL_NONE };

			EGLint numConfigs = 0;

			VTX_TRACE( "[EGL] Calling eglChooseConfig" );
			if ( not eglChooseConfig( _display, configAttribs, &_config, 1, &numConfigs ) || numConfigs == 0 )
			{
				throw std::runtime_error( "EGL: Failed to choose config" );
			}
			VTX_TRACE( "[EGL] eglChooseConfig succeeded with {} config(s)", numConfigs );

			EGLNativeWindowType nativeWindow = reinterpret_cast<EGLNativeWindowType>( p_contextInfo.surface );
			if ( _platform == Platform::Wayland )
			{
				auto * const waylandSurface = reinterpret_cast<wl_surface *>( p_contextInfo.surface );
				if ( waylandSurface == nullptr )
				{
					throw std::runtime_error( "EGL: Wayland platform requires a native surface" );
				}

				_waylandWindow
					= wl_egl_window_create( waylandSurface, static_cast<int>( p_width ), static_cast<int>( p_height ) );
				if ( _waylandWindow == nullptr )
				{
					throw std::runtime_error( "EGL: Failed to create Wayland EGL window" );
				}

				nativeWindow = reinterpret_cast<EGLNativeWindowType>( _waylandWindow );
				VTX_TRACE( "[EGL] Created Wayland EGL window {}", reinterpret_cast<uintptr_t>( _waylandWindow ) );
			}

			VTX_TRACE(
				"[EGL] Calling eglCreateWindowSurface for native window {}", reinterpret_cast<uintptr_t>( nativeWindow )
			);
			const EGLint srgbSurfaceAttribs[] = { EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SRGB_KHR, EGL_NONE };
			_surface				= eglCreateWindowSurface( _display, _config, nativeWindow, srgbSurfaceAttribs );
			_defaultFramebufferSrgb = _surface != EGL_NO_SURFACE;
			if ( _surface == EGL_NO_SURFACE )
			{
				const EGLint srgbError = eglGetError();
				VTX_WARNING(
					"[EGL] Failed to create an sRGB surface (error 0x{:X}), falling back to a linear surface", srgbError
				);
				_surface = eglCreateWindowSurface( _display, _config, nativeWindow, nullptr );
			}
			else
			{
				VTX_INFO( "[EGL] Using an sRGB window surface" );
			}
			VTX_TRACE( "[EGL] eglCreateWindowSurface -> {}", reinterpret_cast<uintptr_t>( _surface ) );

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

			VTX_TRACE( "[EGL] Calling eglCreateContext" );
			_context = eglCreateContext( _display, _config, EGL_NO_CONTEXT, contextAttribs );
			VTX_TRACE( "[EGL] eglCreateContext -> {}", reinterpret_cast<uintptr_t>( _context ) );

			if ( _context == EGL_NO_CONTEXT )
			{
				EGLint			   err = eglGetError();
				std::ostringstream oss;
				oss << "EGL context error: 0x" << std::hex << std::uppercase << err;
				throw std::runtime_error( oss.str() );
			}

			VTX_TRACE( "[EGL] Core profile context {}.{} created", VTX_OPENGL_MAJOR_VERSION, VTX_OPENGL_MINOR_VERSION );
			makeCurrent();
			setSwapInterval( 1 );
		}

		static void * _eglLoader( const char * p_name )
		{
			return reinterpret_cast<void *>( eglGetProcAddress( p_name ) );
		}

		void * getProcAddress() { return reinterpret_cast<void *>( &_eglLoader ); }

		void makeCurrent() const
		{
			VTX_TRACE( "[EGL] Calling eglMakeCurrent" );
			if ( not eglMakeCurrent( _display, _surface, _surface, _context ) )
			{
				throw std::runtime_error( "EGL: makeCurrent failed" );
			}
			VTX_TRACE( "[EGL] eglMakeCurrent succeeded" );
		}

		void swapBuffers() const { eglSwapBuffers( _display, _surface ); }

		bool isDefaultFramebufferSrgb() const { return _defaultFramebufferSrgb; }

		void setSwapInterval( const int p_interval )
		{
			if ( not eglSwapInterval( _display, p_interval ) )
			{
				throw std::runtime_error( "EGL: Failed to set swap interval" );
			}
		}

		void resize( const uint32_t p_width, const uint32_t p_height )
		{
			if ( _waylandWindow != nullptr )
			{
				wl_egl_window_resize( _waylandWindow, static_cast<int>( p_width ), static_cast<int>( p_height ), 0, 0 );
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
			if ( _waylandWindow != nullptr )
			{
				wl_egl_window_destroy( _waylandWindow );
			}

			eglTerminate( _display );
			_display	   = EGL_NO_DISPLAY;
			_surface	   = EGL_NO_SURFACE;
			_context	   = EGL_NO_CONTEXT;
			_config		   = nullptr;
			_nativeDisplay = nullptr;
			_waylandWindow = nullptr;
		}

	  private:
		enum class Platform
		{
			Default,
			X11,
			Wayland
		};

		EGLDisplay		_display				= EGL_NO_DISPLAY;
		EGLSurface		_surface				= EGL_NO_SURFACE;
		EGLContext		_context				= EGL_NO_CONTEXT;
		EGLConfig		_config					= nullptr;
		Platform		_platform				= Platform::Default;
		void *			_nativeDisplay			= nullptr;
		wl_egl_window * _waylandWindow			= nullptr;
		bool			_defaultFramebufferSrgb = false;

		int _major = 0;
		int _minor = 0;
	};

	using GLContextWrapper = EGLContextWrapper;

} // namespace VTX::Renderer::Context::Backend::GL

#endif
#endif
