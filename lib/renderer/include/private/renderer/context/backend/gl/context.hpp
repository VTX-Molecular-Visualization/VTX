#pragma once

#include "include_opengl.hpp"
#include <cstdio>
#include <sstream>
#include <stdexcept>
#include <type_traits>

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
		WGLContextWrapper()										   = default;
		WGLContextWrapper( const WGLContextWrapper & )			   = delete;
		WGLContextWrapper & operator=( const WGLContextWrapper & ) = delete;

		~WGLContextWrapper() { destroy(); }

		void init( const Desc::HeadlessContextInfo & )
		{
			VTX_TRACE( "[WGL] Creating headless context" );

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

			_hwnd = CreateWindowA(
				className, "VTX WGL Headless", WS_OVERLAPPEDWINDOW, 0, 0, 1, 1, nullptr, nullptr, instance, nullptr
			);
			if ( not _hwnd )
			{
				throw std::runtime_error( "WGL: Failed to create headless window" );
			}

			_hdc = GetDC( _hwnd );
			if ( not _hdc )
			{
				throw std::runtime_error( "WGL: Failed to get device context" );
			}

			PIXELFORMATDESCRIPTOR pfd = _defaultPixelFormatDescriptor();

			// Bootstrap WGL extensions from a dummy window/context before configuring the real surface.
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
										   Desc::OPENGL_MAJOR_VERSION,
										   WGL_CONTEXT_MINOR_VERSION_ARB,
										   Desc::OPENGL_MINOR_VERSION,

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
			catch ( ... )
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

			VTX_TRACE(
				"[WGL] Core profile context {}.{} created", Desc::OPENGL_MAJOR_VERSION, Desc::OPENGL_MINOR_VERSION
			);
			makeCurrent();
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

		static void * getProcAddress() { return reinterpret_cast<void *>( &_wglLoader ); }

		void makeCurrent() const
		{
			if ( not wglMakeCurrent( _hdc, _context ) )
			{
				throw std::runtime_error( "WGL: makeCurrent failed" );
			}
		}

		void swapBuffers() const noexcept {}

		static void setSwapInterval( const int ) {}

		void destroy()
		{
			if ( _hdc )
			{
				wglMakeCurrent( nullptr, nullptr );

				if ( _context )
				{
					wglDeleteContext( _context );
					_context = nullptr;
				}

				ReleaseDC( _hwnd, _hdc );
				_hdc = nullptr;
			}

			if ( _hwnd )
			{
				DestroyWindow( _hwnd );
				_hwnd = nullptr;
			}
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
			std::remove_pointer_t<PFNWGLCHOOSEPIXELFORMATARBPROC> * const p_choosePixelFormat,
			PIXELFORMATDESCRIPTOR &										  p_pfd
		)
		{
			int pixelFormat = 0;

			if ( p_choosePixelFormat )
			{
				const int pixelAttribs[ 15 ] = { WGL_DRAW_TO_WINDOW_ARB, 1,	 WGL_SUPPORT_OPENGL_ARB, 1,
												 WGL_DOUBLE_BUFFER_ARB,	 1,	 WGL_PIXEL_TYPE_ARB,	 WGL_TYPE_RGBA_ARB,
												 WGL_COLOR_BITS_ARB,	 32, WGL_DEPTH_BITS_ARB,	 24,
												 WGL_STENCIL_BITS_ARB,	 8 };

				const auto chooseFormat = [ & ]( const int * const p_attributes )
				{
					UINT numFormats = 0;
					pixelFormat		= 0;
					return p_choosePixelFormat( _hdc, p_attributes, nullptr, 1, &pixelFormat, &numFormats )
						   && numFormats > 0;
				};

				if ( chooseFormat( pixelAttribs ) )
				{
					PIXELFORMATDESCRIPTOR chosen = {};
					if ( DescribePixelFormat( _hdc, pixelFormat, sizeof( chosen ), &chosen ) != 0 )
					{
						p_pfd = chosen;
						VTX_TRACE(
							"[WGL] Using ARB pixel format {} (color={}, depth={}, stencil={}, double_buffer={})",
							pixelFormat,
							static_cast<int>( p_pfd.cColorBits ),
							static_cast<int>( p_pfd.cDepthBits ),
							static_cast<int>( p_pfd.cStencilBits ),
							( p_pfd.dwFlags & PFD_DOUBLEBUFFER ) != 0
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

		HWND  _hwnd	   = nullptr;
		HDC	  _hdc	   = nullptr;
		HGLRC _context = nullptr;
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

		void init( const Desc::HeadlessContextInfo & )
		{ throw std::runtime_error( "macOS OpenGL context creation is not implemented yet" ); }

		void * getProcAddress() { return nullptr; }

		void makeCurrent() const { throw std::runtime_error( "macOS OpenGL context is not initialized" ); }

		void swapBuffers() const noexcept {}

		void setSwapInterval( int ) {}

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
#include <array>
#include <renderer/descriptors.hpp>
#include <string_view>
#include <util/logger.hpp>

namespace VTX::Renderer::Context::Backend::GL
{
	class EGLContextWrapper
	{
	  public:
		EGLContextWrapper()										   = default;
		EGLContextWrapper( const EGLContextWrapper & )			   = delete;
		EGLContextWrapper & operator=( const EGLContextWrapper & ) = delete;

		~EGLContextWrapper() { destroy(); }

		void init( const Desc::HeadlessContextInfo & )
		{
			VTX_TRACE( "[EGL] Creating headless context" );

			_display = _initializeHeadlessDisplay();
			if ( _display == EGL_NO_DISPLAY )
			{
				throw std::runtime_error( "EGL: Failed to initialize a headless display" );
			}
			VTX_TRACE( "[EGL] Initialized EGL {}.{}", _major, _minor );

			if ( not eglBindAPI( EGL_OPENGL_API ) )
			{
				throw std::runtime_error( "EGL: Failed to bind OpenGL API" );
			}

			const EGLint configAttribs[] = { EGL_SURFACE_TYPE,
											 EGL_PBUFFER_BIT,
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

			const EGLint pbufferAttribs[] = { EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE };
			_surface					  = eglCreatePbufferSurface( _display, _config, pbufferAttribs );
			if ( _surface == EGL_NO_SURFACE )
			{
				const EGLint	   err = eglGetError();
				std::ostringstream oss;
				oss << "EGL surface error: 0x" << std::hex << std::uppercase << err;
				throw std::runtime_error( oss.str() );
			}

			const EGLint contextAttribs[] = { EGL_CONTEXT_MAJOR_VERSION,
											  Desc::OPENGL_MAJOR_VERSION,
											  EGL_CONTEXT_MINOR_VERSION,
											  Desc::OPENGL_MINOR_VERSION,

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
				const EGLint	   err = eglGetError();
				std::ostringstream oss;
				oss << "EGL context error: 0x" << std::hex << std::uppercase << err;
				throw std::runtime_error( oss.str() );
			}

			VTX_TRACE(
				"[EGL] Core profile context {}.{} created", Desc::OPENGL_MAJOR_VERSION, Desc::OPENGL_MINOR_VERSION
			);
			makeCurrent();
		}

		static void * _eglLoader( const char * p_name )
		{ return reinterpret_cast<void *>( eglGetProcAddress( p_name ) ); }

		void * getProcAddress() { return reinterpret_cast<void *>( &_eglLoader ); }

		void makeCurrent() const
		{
			if ( not eglMakeCurrent( _display, _surface, _surface, _context ) )
			{
				throw std::runtime_error( "EGL: makeCurrent failed" );
			}
		}

		void swapBuffers() const noexcept {}

		void setSwapInterval( const int ) {}

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
			_surface = EGL_NO_SURFACE;
			_context = EGL_NO_CONTEXT;
			_config	 = nullptr;
		}

	  private:
		EGLDisplay _display = EGL_NO_DISPLAY;
		EGLSurface _surface = EGL_NO_SURFACE;
		EGLContext _context = EGL_NO_CONTEXT;
		EGLConfig  _config	= nullptr;

		int _major = 0;
		int _minor = 0;

		bool _tryInitialize( const EGLDisplay p_display, const std::string_view p_name )
		{
			if ( p_display == EGL_NO_DISPLAY )
			{
				return false;
			}
			if ( not eglInitialize( p_display, &_major, &_minor ) )
			{
				VTX_TRACE( "[EGL] {} display initialization failed (0x{:X})", p_name, eglGetError() );
				return false;
			}
			VTX_INFO( "[EGL] Using {} display", p_name );
			return true;
		}

		EGLDisplay _initializeHeadlessDisplay()
		{
			const char * const	   clientExtensions = eglQueryString( EGL_NO_DISPLAY, EGL_EXTENSIONS );
			const std::string_view extensions		= clientExtensions != nullptr ? clientExtensions : "";
			const auto			   hasExtension		= [ & ]( const std::string_view p_name )
			{ return extensions.find( p_name ) != std::string_view::npos; };

			if ( hasExtension( "EGL_MESA_platform_surfaceless" ) )
			{
				const EGLDisplay display
					= eglGetPlatformDisplay( EGL_PLATFORM_SURFACELESS_MESA, EGL_DEFAULT_DISPLAY, nullptr );
				if ( _tryInitialize( display, "surfaceless" ) )
				{
					return display;
				}
			}

			if ( hasExtension( "EGL_EXT_platform_device" ) && hasExtension( "EGL_EXT_device_enumeration" ) )
			{
				const auto queryDevices
					= reinterpret_cast<PFNEGLQUERYDEVICESEXTPROC>( eglGetProcAddress( "eglQueryDevicesEXT" ) );

				std::array<EGLDeviceEXT, 16> devices	 = {};
				EGLint						 deviceCount = 0;
				if ( queryDevices != nullptr
					 && queryDevices( static_cast<EGLint>( devices.size() ), devices.data(), &deviceCount ) )
				{
					for ( EGLint i = 0; i < deviceCount; ++i )
					{
						const EGLDisplay display
							= eglGetPlatformDisplay( EGL_PLATFORM_DEVICE_EXT, devices[ i ], nullptr );
						if ( _tryInitialize( display, "device" ) )
						{
							return display;
						}
					}
				}
			}

			const EGLDisplay display = eglGetDisplay( EGL_DEFAULT_DISPLAY );
			if ( _tryInitialize( display, "default" ) )
			{
				return display;
			}

			return EGL_NO_DISPLAY;
		}
	};

	using GLContextWrapper = EGLContextWrapper;

} // namespace VTX::Renderer::Context::Backend::GL

#endif
