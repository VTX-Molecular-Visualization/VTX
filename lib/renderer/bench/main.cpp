#include "camera.hpp"
#include "user_interface.hpp"
#include "util.hpp"
#include <iostream>
#include <renderer/gl/opengl_renderer.hpp>
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

		Core::StructMolecule molecule = generateAtomGrid( 9 );
		StructProxyMolecule	 proxyMolecule
			= { &molecule.tranform,			&molecule.atomPositions,  &molecule.atomColors, &molecule.atomRadii,
				&molecule.atomVisibilities, &molecule.atomSelections, &molecule.atomIds,	&molecule.bonds };
		renderer.addMolecule( proxyMolecule );

		static bool isRunning = true;
		while ( isRunning )
		{
			float time = float( ui.getTime() ) * 1e-3f;

			// TODO: move in a dedicated SSBO.
			Mat4f modelMatrix = Math::rotate( MAT4F_ID, time * 0.1f, VEC3F_X );
			modelMatrix		  = Math::rotate( modelMatrix, time * 0.2f, VEC3F_Y );
			modelMatrix		  = Math::rotate( modelMatrix, time * 0.05f, VEC3F_Z );
			renderer.setMatrixModelTmp( modelMatrix );

			renderer.renderFrame();
			ui.draw( &renderer, &camera );

			// Events.
			SDL_Event	event;
			static bool keys[ 322 ] = { false };
			while ( ui.getEvent( event ) )
			{
				switch ( event.type )
				{
				case SDL_QUIT: isRunning = false; break;
				case SDL_KEYDOWN: keys[ event.key.keysym.sym ] = true; break;
				case SDL_KEYUP: keys[ event.key.keysym.sym ] = false; break;
				case SDL_WINDOWEVENT:
					switch ( event.window.event )
					{
					case SDL_WINDOWEVENT_RESIZED:
						const size_t width	= event.window.data1;
						const size_t height = event.window.data2;
						renderer.resize( width, height );
						camera.resize( width, height );
						break;
					}
					break;
				default: break;
				}
			}

			// Move.
			Vec3i deltaMoveInputs = Vec3i( 0, 0, 0 );

			if ( keys[ SDLK_z ] )
			{
				deltaMoveInputs.z++;
			}
			if ( keys[ SDLK_s ] )
			{
				deltaMoveInputs.z--;
			}
			if ( keys[ SDLK_q ] )
			{
				deltaMoveInputs.x--;
			}
			if ( keys[ SDLK_d ] )
			{
				deltaMoveInputs.x++;
			}
			if ( keys[ SDLK_r ] )
			{
				deltaMoveInputs.y++;
			}
			if ( keys[ SDLK_f ] )
			{
				deltaMoveInputs.y--;
			}

			if ( deltaMoveInputs != VEC3I_ZERO )
			{
				camera.translate( Vec3f( deltaMoveInputs ) * ui.getDeltaTime() );
			}
		}
	}
	catch ( const std::exception & p_e )
	{
		VTX_ERROR( "{}", p_e.what() );
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
