#include "camera.hpp"
#include "input_manager.hpp"
#include "user_interface.hpp"
#include "util.hpp"
#include <core/chemdb/atom.hpp>
#include <core/chemdb/color.hpp>
#include <core/chemdb/secondary_structure.hpp>
#include <core/struct/molecule.hpp>
#include <io/reader/molecule.hpp>
#include <io/util/secondary_structure.hpp>
#include <iostream>
#include <numeric>
#include <renderer/renderer.hpp>
#include <util/filesystem.hpp>
#include <util/math.hpp>

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
		inputManager.setCallbackClose( [ &isRunning ]() { isRunning = false; } );
		inputManager.setCallbackTranslate( [ &camera, &ui ]( const Vec3i & p_delta )
										   { camera.translate( Vec3f( p_delta ) * ui.getDeltaTime() ); } );
		inputManager.setCallbackRotate( [ &camera, &ui ]( const Vec2i & p_delta )
										{ camera.rotate( Vec3f( -p_delta.y, -p_delta.x, 0.f ) * ui.getDeltaTime() ); }
		);
		inputManager.setCallbackZoom( [ &camera, &ui ]( const int p_delta )
									  { camera.zoom( -float( p_delta ) * ui.getDeltaTime() ); } );
		inputManager.setCallbackResize(
			[ &renderer, &camera ]( const size_t p_width, const size_t p_height )
			{
				renderer.resize( p_width, p_height );
				camera.resize( p_width, p_height );
			}
		);

		renderer.setCallbackClean(
			[ &camera, &inputManager ]()
			{
				camera.setCallbackMatrixView( nullptr );
				camera.setCallbackMatrixProjection( nullptr );
				camera.setCallbackTranslation( nullptr );
				camera.setCallbackClipInfos( nullptr );
				inputManager.setCallbackMouseMotion( nullptr );
			}
		);

		renderer.setCallbackReady(
			[ &ui, &renderer, &camera, &inputManager ]()
			{
				camera.setCallbackMatrixView( [ &renderer ]( const Mat4f & p_matrix )
											  { renderer.setMatrixView( p_matrix ); } );
				camera.setCallbackMatrixProjection( [ &renderer ]( const Mat4f & p_matrix )
													{ renderer.setMatrixProjection( p_matrix ); } );
				camera.setCallbackTranslation( [ &renderer ]( const Vec3f p_position )
											   { renderer.setCameraPosition( p_position ); } );
				camera.setCallbackClipInfos( [ &renderer ]( const float p_near, const float p_far )
											 { renderer.setCameraClipInfos( p_near, p_far ); } );
				inputManager.setCallbackMouseMotion( [ &renderer ]( const Vec2i & p_position )
													 { renderer.setMousePosition( p_position ); } );
			}
		);

		// Model.
		/*
		Molecule							molecule = generateAtomGrid( 9 );
		const Renderer::StructProxyMolecule proxyMolecule
			= { &molecule.transform,		&molecule.atomPositions,  &molecule.atomColors, &molecule.atomRadii,
				&molecule.atomVisibilities, &molecule.atomSelections, &molecule.atomIds,	&molecule.bonds };
		renderer.addMolecule( proxyMolecule );
		*/

		const std::string name = "4hhb.pdb";
		const FilePath	  path = Filesystem::getExecutableDir() / name;

		// Read model file.
		Reader::Molecule			reader;
		VTX::Core::Struct::Molecule molecule;
		reader.readFile( path, molecule );

		VTX::IO::Util::SecondaryStructure::computeStride( molecule );

		// Proxify.
		// Move or maybe redo.

		size_t										   size	   = molecule.trajectory.frames.front().size();
		std::vector<VTX::Core::ChemDB::Atom::SYMBOL> & symbols = molecule.atomSymbols;
		std::vector<uchar>							   colors  = std::vector<uchar>( size );
		std::generate( colors.begin(), colors.end(), [] { return rand() % 256; } );
		std::vector<float> radii( size );
		std::generate(
			radii.begin(),
			radii.end(),
			[ &symbols ]
			{
				static int i = 0;
				return VTX::Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ int( symbols[ i++ ] ) ];
			}
		);
		std::vector<bool> visibilities = std::vector<bool>( size, true );
		std::vector<bool> selections   = std::vector<bool>( size, false );
		std::vector<uint> ids( size );
		std::iota( ids.begin(), ids.end(), 0 );

		std::vector<uint> bondsIndex( molecule.bondPairAtomIndexes.size() );

		const std::vector<atom_index_t> & bondPairAtomIndexes = molecule.bondPairAtomIndexes;
		std::generate(
			bondsIndex.begin(),
			bondsIndex.end(),
			[ &bondPairAtomIndexes ]
			{
				static size_t i = 0;
				return uint( bondPairAtomIndexes[ i++ ] );
			}
		);

		Renderer::StructProxyMolecule proxyMolecule = {
			&molecule.transform, &molecule.trajectory.frames.front(), &colors, &radii, &visibilities, &selections, &ids,
			&bondsIndex
		};
		renderer.addMolecule( proxyMolecule );

		renderer.build();

		// Generate array of random colors.
		VTX::Core::ChemDB::Color::ColorLayout colorLayout;
		std::generate( colorLayout.begin(), colorLayout.end(), [] { return VTX::Util::Color::Rgba::random(); } );
		renderer.setColorLayout( colorLayout );

		// Main loop.
		while ( isRunning )
		{
			float time = float( ui.getTime() ) * 1e-3f;

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
