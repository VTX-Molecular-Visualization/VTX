#include "camera.hpp"
#include "input_manager.hpp"
#include "user_interface.hpp"
#include "util.hpp"
#include <core/chemdb/atom.hpp>
#include <core/flat_struct/molecule.hpp>
#include <io/reader/flat_molecule.hpp>
#include <iostream>
#include <numeric>
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
	using namespace IO;
	using namespace Renderer::GL;
	using namespace Bench;

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
		// Core::Gpu::Molecule molecule = generateAtomGrid( 9 );
		// 		StructProxyMolecule proxyMolecule
		// 			= { &molecule.transform,		&molecule.atomPositions,  &molecule.atomColors, &molecule.atomRadii,
		// 				&molecule.atomVisibilities, &molecule.atomSelections, &molecule.atomIds,	&molecule.bonds };
		// 		renderer.addMolecule( proxyMolecule );
		try
		{
			const std::string name = "4v6x.mmtf";
			const FilePath	  path = std::filesystem::current_path() / name;

			// Read model file.
			Reader::FlatMolecule			reader;
			VTX::Core::FlatStruct::Molecule molecule;
			reader.readFile( path, molecule );

			// Proxify.
			// Move or maybe redo.
			float timeProxify = Util::CHRONO_CPU(
				[ & ]()
				{
					size_t										   size	   = molecule.trajectory.frames.front().size();
					std::vector<VTX::Core::ChemDB::Atom::SYMBOL> & symbols = molecule.atomSymbols;
					std::vector<Color::Rgba>					   colors  = std::vector<Color::Rgba>( size );
					std::generate( colors.begin(), colors.end(), [] { return Color::Rgba::random(); } );
					std::vector<float> radii( size );
					std::generate( radii.begin(),
								   radii.end(),
								   [ &symbols ]
								   {
									   static int i = 0;
									   return VTX::Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ int( symbols[ i++ ] ) ];
								   } );
					std::vector<uint> visibilities = std::vector<uint>( size, 1 );
					std::vector<uint> selections   = std::vector<uint>( size, 0 );
					std::vector<uint> ids( size );
					std::iota( ids.begin(), ids.end(), 0 );
					size_t			  bondsCount = molecule.bondOrders.size();
					std::vector<uint> bondsIndex( bondsCount * 2 );
					for ( uint i = 0; i < bondsCount; ++i )
					{
						bondsIndex[ i * 2 + 0 ] = uint( molecule.bondFirstAtomIndexes[ i ] );
						bondsIndex[ i * 2 + 1 ] = uint( molecule.bondSecondAtomIndexes[ i ] );
					}

					// TODO:
					// Use struct ssbo for atom infos by symbol (like radius).
					// Setup color ssbo and layout.
					// Setup representation ssbo and layout.
					// Persisted data in CPU cache with smart ptr?
					StructProxyMolecule proxyMolecule = { &molecule.transform,
														  &molecule.trajectory.frames.front(),
														  &colors,
														  &radii,
														  &visibilities,
														  &selections,
														  &ids,
														  &bondsIndex };
					renderer.addMolecule( proxyMolecule );
				} );
			VTX_INFO( "Proxify time: {}", timeProxify );
		}
		catch ( const std::exception & p_e )
		{
			VTX_ERROR( "{}", p_e.what() );
		}

		// Main loop.
		while ( isRunning )
		{
			float time = float( ui.getTime() ) * 1e-3f;

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
