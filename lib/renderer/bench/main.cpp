#include "camera.hpp"
#include "input_manager.hpp"
#include "user_interface.hpp"
#include "util.hpp"
#include <iostream>
#include <renderer/gl/opengl_renderer.hpp>
#include <renderer/render_graph.hpp>
#include <util/math.hpp>

#ifdef _WIN32
extern "C"
{
	__declspec( dllexport ) uint32_t NvOptimusEnablement			 = 0x00000001;
	__declspec( dllexport ) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

constexpr size_t WIDTH	= 1920;
constexpr size_t HEIGHT = 1200;

int main( int, char ** )
{
	using namespace VTX;
	using namespace Util;
	using namespace Bench;
	using namespace Renderer::GL;

	bool isRunning = true;
	Logger::get().init( std::filesystem::current_path() / "logs" );

	try
	{
		// UI.
		UserInterface ui( WIDTH, HEIGHT );

		// Renderer.
		OpenGLRenderer renderer( ui.getProcAddress(), WIDTH, HEIGHT, std::filesystem::current_path() / "shaders" / "" );

		// Camera.
		Camera camera( WIDTH, HEIGHT );
		camera.setCallbackMatrixView( [ &renderer ]( const Mat4f & p_matrix ) { renderer.setMatrixView( p_matrix ); } );
		camera.setCallbackMatrixProjection( [ &renderer ]( const Mat4f & p_matrix )
											{ renderer.setMatrixProjection( p_matrix ); } );
		camera.setCallbackClipInfos( [ &renderer ]( const float p_near, const float p_far )
									 { renderer.setCameraClipInfos( p_near, p_far ); } );

		// Input manager.
		InputManager inputManager;
		inputManager.setCallbackClose( [ &isRunning ]() { isRunning = false; } );
		inputManager.setCallbackResize(
			[ &renderer, &camera ]( const size_t p_width, const size_t p_height )
			{
				renderer.resize( p_width, p_height );
				camera.resize( p_width, p_height );
			} );
		inputManager.setCallbackTranslate( [ &camera, &ui ]( const Vec3i & p_delta )
										   { camera.translate( Vec3f( p_delta ) * ui.getDeltaTime() ); } );
		inputManager.setCallbackRotate(
			[ &camera, &ui ]( const Vec2i & p_delta )
			{ camera.rotate( Vec3f( -p_delta.y, -p_delta.x, 0.f ) * ui.getDeltaTime() ); } );
		inputManager.setCallbackZoom( [ &camera, &ui ]( const int p_delta )
									  { camera.zoom( -float( p_delta ) * ui.getDeltaTime() ); } );

		// Model.
		Core::Gpu::Molecule molecule = generateAtomGrid( 9 );
		StructProxyMolecule proxyMolecule
			= { &molecule.transform,		&molecule.atomPositions,  &molecule.atomColors, &molecule.atomRadii,
				&molecule.atomVisibilities, &molecule.atomSelections, &molecule.atomIds,	&molecule.bonds };
		renderer.addMolecule( proxyMolecule );

		// Main loop.
		while ( isRunning )
		{
			float time = float( ui.getTime() ) * 1e-3f;
			//
			// 			// TODO: move in a dedicated SSBO.
			// 			Mat4f modelMatrix = Math::rotate( MAT4F_ID, time * 0.1f, VEC3F_X );
			// 			modelMatrix		  = Math::rotate( modelMatrix, time * 0.2f, VEC3F_Y );
			// 			modelMatrix		  = Math::rotate( modelMatrix, time * 0.05f, VEC3F_Z );
			// 			renderer.setMatrixModelTmp( modelMatrix );

			// Renderer.
			renderer.renderFrame();

			// UI.
			ui.draw( &renderer, &camera );

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
