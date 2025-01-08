#include "input_manager.hpp"
#include "scene.hpp"
#include "user_interface.hpp"
#include <iostream>
#include <renderer/facade.hpp>
#include <renderer/proxy/representation.hpp>
#include <renderer/proxy/voxels.hpp>
#include <util/math/aabb.hpp>

#ifdef _WIN32
extern "C"
{
	__declspec( dllexport ) uint32_t NvOptimusEnablement				  = 0x00000001;
	__declspec( dllexport ) int		 AmdPowerXpressRequestHighPerformance = 1;
}
#endif

constexpr size_t WIDTH	= 1920;
constexpr size_t HEIGHT = 1080;

int main( int, char ** )
{
	using namespace VTX;
	using namespace Util;
	using namespace Bench;

	bool isRunning = true;
	LOGGER::init( Filesystem::getExecutableDir() / "logs", true );

	try
	{
		// Scene.
		Scene	 scene( WIDTH, HEIGHT );
		Camera & camera = scene.getCamera();

		// UI.
		UserInterface ui( WIDTH, HEIGHT );

		// Renderer.
		Renderer::Facade renderer( WIDTH, HEIGHT, Filesystem::getExecutableDir() / "shaders", ui.getProcAddress() );
		renderer.build();
		renderer.setProxyCamera( scene.getProxyCamera() );

		// Input manager.
		InputManager inputManager;
		inputManager.onClose += [ &isRunning ]() { isRunning = false; };
		inputManager.onTranslate +=
			[ &camera, &ui ]( const Vec3i & p_delta ) { camera.translate( Vec3f( p_delta ) * ui.getDeltaTime() ); };
		inputManager.onRotate += [ &camera, &ui ]( const Vec2i & p_delta )
		{ camera.rotate( Vec3f( -p_delta.y, -p_delta.x, 0.f ) * ui.getDeltaTime() ); };
		inputManager.onZoom +=
			[ &camera, &ui ]( const int p_delta ) { camera.zoom( -float( p_delta ) * ui.getDeltaTime() ); };

		inputManager.onResize += [ &renderer, &camera ]( const size_t p_width, const size_t p_height )
		{
			renderer.resize( p_width, p_height );
			camera.resize( p_width, p_height );
		};
		inputManager.onRestore += [ &renderer ]() { renderer.setNeedUpdate( true ); };
		inputManager.onMousePick += [ &renderer ]( const size_t p_x, const size_t p_y )
		{
			Vec2i ids = renderer.getPickedIds( p_x, p_y );
			VTX_DEBUG( "Picked ids: {} {}", ids.x, ids.y );
		};
		inputManager.onMouseMotion +=
			[ & ]( const Vec2i & p_position ) { scene.getProxyCamera().onMousePosition( p_position ); };

		inputManager.onKeyPressed += [ & ]( const SDL_Scancode p_key )
		{
			try
			{
				if ( p_key == SDL_SCANCODE_F1 )
				{
					renderer.addProxySystem( scene.addSystem( "4hhb" ) );
				}
				else if ( p_key == SDL_SCANCODE_F2 )
				{
					renderer.addProxySystem( scene.addSystem( "1aga" ) );
				}
				else if ( p_key == SDL_SCANCODE_F3 )
				{
					renderer.addProxySystem( scene.addSystem( "4v6x" ) );
				}
				else if ( p_key == SDL_SCANCODE_F4 )
				{
					renderer.addProxySystem( scene.addSystem( "3j3q.mmtf" ) );
				}
				else if ( p_key == SDL_SCANCODE_F5 )
				{
					VTX::Core::Struct::ColorLayout colorLayout;
					std::generate(
						colorLayout.layout.begin(), colorLayout.layout.end(), [] { return Color::Rgba::random(); }
					);
					scene.setColorLayout( colorLayout );
				}
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "{}", p_e.what() );
			}
		};

		inputManager.onFileDrop += [ & ]( const FilePath & p_filePath )
		{
			try
			{
				renderer.addProxySystem( scene.addSystem( p_filePath.string() ) );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "{}", p_e.what() );
			}
		};

		// Math::AABB aabb( VEC3F_ZERO, 100.f );

		// std::vector<Vec3f> mins		   = { aabb.getMin() };
		// std::vector<Vec3f> maxs		   = { aabb.getMax() };

		// Generate grid with mins and maxs.
		std::vector<Vec3f> mins, maxs;
		for ( float x = -100.f; x <= 100.f; x += 50.f )
		{
			for ( float y = -100.f; y <= 100.f; y += 50.f )
			{
				for ( float z = -100.f; z <= 100.f; z += 50.f )
				{
					mins.emplace_back( x, y, z );
					maxs.emplace_back( x + 50.f, y + 50.f, z + 50.f );
				}
			}
		}

		auto proxyVoxels = Renderer::Proxy::Voxels { &mins, &maxs };
		renderer.setProxyVoxels( proxyVoxels );

		renderer.setProxyColorLayout( scene.getProxyColorLayout() );

		Renderer::Proxy::Representation representation1, representation2, representation3;

		representation1.set( Renderer::Proxy::E_REPRESENTATION_SETTINGS::HAS_SPHERE, true );
		representation1.set( Renderer::Proxy::E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_FIXED, 0.5f );
		representation1.set( Renderer::Proxy::E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_ADD, 0.f );
		representation1.set( Renderer::Proxy::E_REPRESENTATION_SETTINGS::IS_SPHERE_RADIUS_FIXED, true );

		representation1.set( Renderer::Proxy::E_REPRESENTATION_SETTINGS::HAS_CYLINDER, true );
		representation1.set( Renderer::Proxy::E_REPRESENTATION_SETTINGS::RADIUS_CYLINDER, 0.1f );
		representation1.set( Renderer::Proxy::E_REPRESENTATION_SETTINGS::CYLINDER_COLOR_BLENDING, false );

		representation1.set( Renderer::Proxy::E_REPRESENTATION_SETTINGS::HAS_RIBBON, true );
		representation1.set( Renderer::Proxy::E_REPRESENTATION_SETTINGS::RIBBON_COLOR_BLENDING, true );

		representation1.set( Renderer::Proxy::E_REPRESENTATION_SETTINGS::HAS_SES, false );

		representation2 = representation1;
		representation3 = representation1;

		representation2.set( Renderer::Proxy::E_REPRESENTATION_SETTINGS::IS_SPHERE_RADIUS_FIXED, false );
		representation3.set( Renderer::Proxy::E_REPRESENTATION_SETTINGS::RADIUS_SPHERE_ADD, 1.5f );

		std::vector<Renderer::Proxy::Representation *> representations
			= { &representation1, &representation2, &representation3 };
		renderer.addProxyRepresentations( representations );

		// Renderer::Proxy::RenderSettings renderSettings
		//	= { 6.f, 18.f,	 COLOR_WHITE, COLOR_YELLOW, COLOR_BLACK, 2,	  1.f, 1.f,
		//		3,	 1000.f, 1000.f,	  0.5f,			COLOR_RED,	 1.f, 1,   COLOR_BLUE };

		// renderer.setProxyRenderSettings( renderSettings );

		// Main loop.
		while ( isRunning )
		{
			float time		= float( ui.getTime() );
			float deltaTime = ui.getDeltaTime();

			// Update scene.
			scene.update( deltaTime );

			// Renderer.
			renderer.render( deltaTime, time );

			// UI.
			ui.draw( &camera, &scene, &renderer );

			// Events.
			SDL_Event event;
			while ( ui.getEvent( event ) )
			{
				inputManager.handle( event );
			}

			inputManager.update();
			inputManager.consumeInputs();
		}
	}
	catch ( const std::exception & p_e )
	{
		VTX_ERROR( "Unhandled exception: {}", p_e.what() );
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
