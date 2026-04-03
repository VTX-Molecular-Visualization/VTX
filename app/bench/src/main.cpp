#include "input_manager.hpp"
#include "scene.hpp"
#include "user_interface.hpp"
#include <iostream>
#include <renderer/graphics_config.hpp>
#include <renderer/renderer.hpp>
#include <renderer/representation.hpp>
#include <util/filesystem.hpp>
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

	try
	{
		LOGGER::init( Filesystem::getExecutableDir() / "logs", true );

		// Scene.
		Scene			   scene( WIDTH, HEIGHT );
		CameraController & camera = scene.getCamera();

		// UI.
		UserInterface ui( WIDTH, HEIGHT );

		// Renderer.
		Renderer::Renderer				  renderer( WIDTH, HEIGHT );
		Renderer::Desc::NativeContextInfo contextInfo;
		contextInfo.surface	  = ui.getNativeSurface();
		contextInfo.display	  = ui.getNativeDisplay();
		contextInfo.plateform = static_cast<Renderer::Desc::E_NATIVE_PLATEFORM>( ui.getNativePlatform() );

		renderer.setOpenGL( contextInfo, Filesystem::getExecutableDir() / "shaders" );
		renderer.setGraphicsConfig( Renderer::GraphicsConfigs::DEFAULT );
		renderer.setColorLayout( Renderer::Color::Layouts::JMOL );
		renderer.setRepresentations( { &Renderer::Representations::STICKS_AND_RIBBONS } );
		// renderer.setVSync( false );
		scene.syncRenderer( renderer );

		// Input manager.
		InputManager inputManager;

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
		renderer.setVoxels( mins, maxs );

		float previousTime = float( ui.getTime() ) * 1e-3f;

		// Main loop.
		while ( isRunning )
		{
			SDL_Event event;
			while ( ui.getEvent( event ) )
			{
				inputManager.handle( event );
			}

			const float time	  = float( ui.getTime() ) * 1e-3f;
			const float deltaTime = time - previousTime;
			previousTime		  = time;

			inputManager.update();
			const FrameInputs frameInputs		  = inputManager.consumeInputs();
			const auto		  syncSceneToRenderer = [ &renderer, &scene ]() { scene.syncRenderer( renderer ); };

			isRunning = not frameInputs.closeRequested;

			if ( frameInputs.resize.has_value() )
			{
				renderer.resize( frameInputs.resize->width, frameInputs.resize->height );
				camera.resize( frameInputs.resize->width, frameInputs.resize->height );
			}
			if ( frameInputs.restoreRequested )
			{
				renderer.setNeedUpdate( true );
			}
			if ( frameInputs.translation != VEC3I_ZERO )
			{
				camera.translate( Vec3f( frameInputs.translation ) * deltaTime );
			}
			if ( frameInputs.rotation != VEC2I_ZERO )
			{
				camera.rotate( Vec3f( -frameInputs.rotation.y, -frameInputs.rotation.x, 0.f ) * deltaTime );
			}
			if ( frameInputs.zoom != 0 )
			{
				camera.zoom( -float( frameInputs.zoom ) * deltaTime );
			}

			if ( frameInputs.mousePick.has_value() )
			{
				Vec2i ids = renderer.getPickedIds( frameInputs.mousePick->x, frameInputs.mousePick->y );
				VTX_DEBUG( "Picked ids: {} {}", ids.x, ids.y );
			}

			for ( const SDL_Scancode key : frameInputs.pressedKeys )
			{
				try
				{
					if ( key == SDL_SCANCODE_F1 )
					{
						scene.addSystem( "4hhb" );
						syncSceneToRenderer();
					}
					else if ( key == SDL_SCANCODE_F2 )
					{
						scene.addSystem( "1aga" );
						syncSceneToRenderer();
					}
					else if ( key == SDL_SCANCODE_F3 )
					{
						scene.addSystem( "4v6x" );
						syncSceneToRenderer();
					}
					else if ( key == SDL_SCANCODE_F4 )
					{
						scene.addSystem( "3j3q.mmtf" );
						syncSceneToRenderer();
					}
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "{}", p_e.what() );
				}
			}

			for ( const FilePath & filePath : frameInputs.droppedFiles )
			{
				try
				{
					scene.addSystem( filePath.string() );
					syncSceneToRenderer();
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "{}", p_e.what() );
				}
			}

			// Update scene.
			scene.update( deltaTime );

			renderer.setCamera(
				camera.getCamera(), camera.getPosition(), camera.computeViewMatrix(), camera.computeProjectionMatrix()
			);

			// Renderer.
			renderer.render( deltaTime, time );

			// UI.
			ui.draw( &camera, &scene, &renderer );
		}
	}
	catch ( const std::exception & p_e )
	{
		VTX_ERROR( "Unhandled exception: {}", p_e.what() );
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
