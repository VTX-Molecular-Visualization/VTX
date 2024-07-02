#include "input_manager.hpp"
#include "user_interface.hpp"
#include <iostream>
#include <renderer/facade.hpp>
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
	using namespace IO;
	using namespace Bench;
	using namespace VTX::Core;
	using namespace VTX::Core::Struct;

	bool isRunning = true;
	LOGGER().init( Filesystem::getExecutableDir() / "logs" );

	try
	{
		// UI.
		UserInterface ui( WIDTH, HEIGHT );

		// Renderer.
		Renderer::Renderer renderer( WIDTH, HEIGHT, Filesystem::getExecutableDir() / "shaders", ui.getProcAddress() );
		renderer.build();

		// Camera.
		Camera					camera( WIDTH, HEIGHT );
		Renderer::Proxy::Camera proxyCamera { camera.getMatrixViewPtr(), camera.getMatrixProjectionPtr(),
											  camera.getPosition(),		 VEC2I_ZERO,
											  camera.getNear(),			 camera.getFar(),
											  camera.isPerspective() };
		renderer.setProxyCamera( proxyCamera );
		camera.callbackMatrixView += [ & ]( const Mat4f & p_matrix ) { proxyCamera.onMatrixView(); };
		camera.callbackMatrixProjection += [ & ]( const Mat4f & p_matrix ) { proxyCamera.onMatrixProjection(); };
		camera.callbackTranslation += [ & ]( const Vec3f p_position ) { proxyCamera.onCameraPosition( p_position ); };
		camera.callbackClipInfos +=
			[ & ]( const float p_near, const float p_far ) { proxyCamera.onCameraNearFar( p_near, p_far ); };
		camera.callbackPerspective +=
			[ & ]( const bool p_isPerspective ) { proxyCamera.onPerspective( p_isPerspective ); };

		// Input manager.
		InputManager inputManager;
		inputManager.callbackClose += [ &isRunning ]() { isRunning = false; };
		inputManager.callbackTranslate +=
			[ &camera, &ui ]( const Vec3i & p_delta ) { camera.translate( Vec3f( p_delta ) * ui.getDeltaTime() ); };
		inputManager.callbackRotate += [ &camera, &ui ]( const Vec2i & p_delta )
		{ camera.rotate( Vec3f( -p_delta.y, -p_delta.x, 0.f ) * ui.getDeltaTime() ); };
		inputManager.callbackZoom +=
			[ &camera, &ui ]( const int p_delta ) { camera.zoom( -float( p_delta ) * ui.getDeltaTime() ); };

		inputManager.callbackResize += [ &renderer, &camera ]( const size_t p_width, const size_t p_height )
		{
			renderer.resize( p_width, p_height );
			camera.resize( p_width, p_height );
		};
		inputManager.callbackRestore += [ &renderer ]() { renderer.setNeedUpdate( true ); };
		inputManager.callbackMousePick += [ &renderer ]( const size_t p_x, const size_t p_y )
		{
			if ( renderer.hasContext() )
			{
				Vec2i ids = renderer.getPickedIds( p_x, p_y );
				VTX_DEBUG( "Picked ids: {} {}", ids.x, ids.y );
			}
		};
		inputManager.callbackMouseMotion +=
			[ & ]( const Vec2i & p_position ) { proxyCamera.onMousePosition( p_position ); };

		// Scene.
		Scene scene;

		inputManager.callbackKeyPressed += [ & ]( const SDL_Scancode p_key )
		{
			if ( p_key == SDL_SCANCODE_F1 )
			{
				renderer.addProxyMolecule( scene.addMolecule( "4hhb.pdb" ) );
			}
			else if ( p_key == SDL_SCANCODE_F2 )
			{
				renderer.addProxyMolecule( scene.addMolecule( "1aga.pdb" ) );
			}
			else if ( p_key == SDL_SCANCODE_F3 )
			{
				renderer.addProxyMolecule( scene.addMolecule( "4v6x.mmtf" ) );
			}
			else if ( p_key == SDL_SCANCODE_F4 )
			{
				renderer.addProxyMolecule( scene.addMolecule( "3j3q.mmtf" ) );
			}
			else if ( p_key == SDL_SCANCODE_F5 )
			{
				VTX::Core::Struct::ColorLayout colorLayout;
				std::generate(
					colorLayout.layout.begin(), colorLayout.layout.end(), [] { return Color::Rgba::random(); }
				);
				scene.setColorLayout( colorLayout );
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

		representation2.radiusFixed		  = false;
		representation3.radiusSphereFixed = 1.5f;

		std::vector<Renderer::Proxy::Representation *> representations
			= { &representation1, &representation2, &representation3 };
		renderer.addProxyRepresentations( representations );

		Renderer::Proxy::RenderSettings renderSettings
			= { 6.f, 18.f,	 COLOR_WHITE, COLOR_YELLOW, COLOR_BLACK, 2,	  1.f, 1.f,
				3,	 1000.f, 1000.f,	  0.5f,			COLOR_RED,	 1.f, 1,   COLOR_BLUE };

		// renderer.setProxyRenderSettings( renderSettings );

		renderer.addProxyMolecule( scene.addMolecule( "4hhb" ) );
		// renderer.addProxyMolecule( scene.addMolecule( "2ama_1_npt.trr" ) );

		// Main loop.
		while ( isRunning )
		{
			float time		= float( ui.getTime() ) * 1e-3f;
			float deltaTime = ui.getDeltaTime();

			// Update scene.
			scene.update( deltaTime );

			// Renderer.
			renderer.render( time );

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
