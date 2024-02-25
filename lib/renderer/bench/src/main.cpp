#include "camera.hpp"
#include "input_manager.hpp"
#include "user_interface.hpp"
#include "util.hpp"
#include <iostream>
#include <numeric>
#include <renderer/renderer.hpp>
#include <util/math.hpp>
#include <util/math/aabb.hpp>

#ifdef _WIN32
extern "C"
{
	__declspec( dllexport ) uint32_t NvOptimusEnablement			 = 0x00000001;
	__declspec( dllexport ) int AmdPowerXpressRequestHighPerformance = 1;
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
	Logger::get().init( Filesystem::getExecutableDir() / "logs" );

	try
	{
		// UI.
		UserInterface ui( WIDTH, HEIGHT );

		// Renderer.
		Renderer::Renderer renderer( WIDTH, HEIGHT, Filesystem::getExecutableDir() / "shaders", ui.getProcAddress() );

		// Camera.
		Camera camera( WIDTH, HEIGHT );

		// Input manager.
		InputManager inputManager;

		// Setup callbacks.
		inputManager.callbackClose += [ &isRunning ]() { isRunning = false; };
		inputManager.callbackTranslate +=
			[ &camera, &ui ]( const Vec3i & p_delta ) { camera.translate( Vec3f( p_delta ) * ui.getDeltaTime() ); };
		inputManager.callbackRotate += [ &camera, &ui ]( const Vec2i & p_delta )
		{ camera.rotate( Vec3f( -p_delta.y, -p_delta.x, 0.f ) * ui.getDeltaTime() ); };
		inputManager.callbackZoom +=
			[ &camera, &ui ]( const int p_delta ) { camera.zoom( -float( p_delta ) * ui.getDeltaTime() ); };

		const Callback<Mat4f>::Func *		   cbMatrixView		  = nullptr;
		const Callback<Mat4f>::Func *		   cbMatrixProjection = nullptr;
		const Callback<Vec3f>::Func *		   cbTranslation	  = nullptr;
		const Callback<float, float>::Func *   cbClipInfos		  = nullptr;
		const Callback<bool>::Func *		   cbPerspective	  = nullptr;
		const Callback<size_t, size_t>::Func * cbResize			  = nullptr;
		const Callback<>::Func *			   cbRestore		  = nullptr;
		const Callback<size_t, size_t>::Func * cbMousePick		  = nullptr;
		const Callback<Vec2i>::Func *		   cbMouseMotion	  = nullptr;

		// Link camera and input manager with renderer when built.
		renderer.addCallbackReady(
			[ & ]()
			{
				renderer.setMatrixView( camera.getMatrixView() );
				renderer.setMatrixProjection( camera.getMatrixProjection() );
				renderer.setCameraPosition( camera.getPosition() );
				renderer.setCameraClipInfos( camera.getNear(), camera.getFar() );
				renderer.setPerspective( camera.isPerspective() );

				cbMatrixView = camera.callbackMatrixView +=
					[ &renderer ]( const Mat4f & p_matrix ) { renderer.setMatrixView( p_matrix ); };
				cbMatrixProjection = camera.callbackMatrixProjection +=
					[ &renderer ]( const Mat4f & p_matrix ) { renderer.setMatrixProjection( p_matrix ); };
				cbTranslation = camera.callbackTranslation +=
					[ &renderer ]( const Vec3f p_position ) { renderer.setCameraPosition( p_position ); };
				cbClipInfos = camera.callbackClipInfos += [ &renderer ]( const float p_near, const float p_far )
				{ renderer.setCameraClipInfos( p_near, p_far ); };
				cbPerspective = camera.callbackPerspective +=
					[ &renderer ]( const bool p_isPerspective ) { renderer.setPerspective( p_isPerspective ); };

				cbResize = inputManager.callbackResize +=
					[ &renderer, &camera ]( const size_t p_width, const size_t p_height )
				{
					renderer.resize( p_width, p_height );
					camera.resize( p_width, p_height );
				};
				cbRestore	= inputManager.callbackRestore += [ &renderer ]() { renderer.setNeedUpdate( true ); };
				cbMousePick = inputManager.callbackMousePick += [ &renderer ]( const size_t p_x, const size_t p_y )
				{
					Vec2i ids = renderer.getPickedIds( p_x, p_y );
					VTX_DEBUG( "Picked ids: {} {}", ids.x, ids.y );
				};
				cbMouseMotion = inputManager.callbackMouseMotion +=
					[ &renderer ]( const Vec2i & p_position ) { renderer.setMousePosition( p_position ); };
			}
		);

		// Unlink when cleaned.
		renderer.addCallbackClean(
			[ & ]()
			{
				assert( cbMatrixView );
				assert( cbMatrixProjection );
				assert( cbTranslation );
				assert( cbClipInfos );
				assert( cbPerspective );
				assert( cbResize );
				assert( cbRestore );
				assert( cbMousePick );
				assert( cbMouseMotion );

				camera.callbackMatrixView -= cbMatrixView;
				camera.callbackMatrixProjection -= cbMatrixProjection;
				camera.callbackTranslation -= cbTranslation;
				camera.callbackClipInfos -= cbClipInfos;
				camera.callbackPerspective -= cbPerspective;
				inputManager.callbackResize -= cbResize;
				inputManager.callbackRestore -= cbRestore;
				inputManager.callbackMousePick -= cbMousePick;
				inputManager.callbackMouseMotion -= cbMouseMotion;
			}
		);

		// Build renderer.
		renderer.build();

		// Models/proxies.
		std::vector<std::unique_ptr<Molecule>>					molecules;
		std::vector<std::unique_ptr<Renderer::Proxy::Molecule>> proxies;

		auto addMolecule = [ & ]( const std::string & p_name )
		{
			if ( p_name.find( '.' ) != std::string::npos )
			{
				molecules.emplace_back( std::make_unique<Molecule>( loadMolecule( p_name ) ) );
			}
			else
			{
				molecules.emplace_back( std::make_unique<Molecule>( downloadMolecule( p_name ) ) );
			}

			molecules.back()->transform
				= Math::translate( molecules.back()->transform, Math::randomVec3f() * 200.f - 100.f );
			IO::Util::SecondaryStructure::computeStride( *molecules.back() );
			proxies.emplace_back( std::make_unique<Renderer::Proxy::Molecule>( proxify( *molecules.back() ) ) );
			renderer.addProxyMolecule( *proxies.back() );
		};

		addMolecule( "4hhb" );

		inputManager.callbackKeyPressed += [ & ]( const SDL_Scancode p_key )
		{
			if ( p_key == SDL_SCANCODE_F1 )
			{
				addMolecule( "4hhb.pdb" );
			}
			else if ( p_key == SDL_SCANCODE_F2 )
			{
				addMolecule( "1aga.pdb" );
			}
			else if ( p_key == SDL_SCANCODE_F3 )
			{
				addMolecule( "4v6x.mmtf" );
			}
		};

		Math::AABB aabb( VEC3F_ZERO, 100.f );

		std::vector<Vec3f> mins		   = { aabb.getMin() };
		std::vector<Vec3f> maxs		   = { aabb.getMax() };
		auto			   proxyVoxels = Renderer::Proxy::Voxels { &mins, &maxs };
		renderer.setProxyVoxels( proxyVoxels );

		renderer.setProxyColorLayout( ChemDB::Color::COLOR_LAYOUT_JMOL );

		// Main loop.
		while ( isRunning )
		{
			float time		= float( ui.getTime() ) * 1e-3f;
			float deltaTime = ui.getDeltaTime();

			// Update scene.
			if ( ui.isUpdateScene() )
			{
				int i = 0;
				for ( auto & molecule : molecules )
				{
					molecule->transform = Math::rotate( molecule->transform, deltaTime, Math::randomVec3f() );
					proxies[ i++ ]->onTransform();
				}
			}

			// Renderer.
			renderer.render( time );

			// UI.
			ui.draw( &camera, &renderer );

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
