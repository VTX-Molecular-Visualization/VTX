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
#include <util/network.hpp>

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
		inputManager.setCallbackRestore( [ &renderer ]() { renderer.setNeedUpdate( true ); } );
		inputManager.setCallbackMousePick(
			[ &renderer ]( const size_t p_x, const size_t p_y )
			{
				Vec2i ids = renderer.getPickedIds( p_x, p_y );
				VTX_DEBUG( "Picked ids: {} {}", ids.x, ids.y );
			}
		);

		renderer.setCallbackClean(
			[ &camera, &inputManager ]()
			{
				camera.setCallbackMatrixView( nullptr );
				camera.setCallbackMatrixProjection( nullptr );
				camera.setCallbackTranslation( nullptr );
				camera.setCallbackClipInfos( nullptr );
				camera.setCallbackPerspective( nullptr );
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
				camera.setCallbackPerspective( [ &renderer ]( const bool p_isPerspective )
											   { renderer.setPerspective( p_isPerspective ); } );
				inputManager.setCallbackMouseMotion( [ &renderer ]( const Vec2i & p_position )
													 { renderer.setMousePosition( p_position ); } );
			}
		);

		renderer.setCallbackSnapshotPre( [ &camera ]( const size_t p_width, const size_t p_height )
										 { camera.resize( p_width, p_height ); } );

		renderer.setCallbackSnapshotPost( [ &camera ]( const size_t p_width, const size_t p_height )
										  { camera.resize( p_width, p_height ); } );

		// Model.
		/*
		Molecule							molecule = generateAtomGrid( 9 );
		const Renderer::StructProxyMolecule proxyMolecule
			= { &molecule.transform,		&molecule.atomPositions,  &molecule.atomColors, &molecule.atomRadii,
				&molecule.atomVisibilities, &molecule.atomSelections, &molecule.atomIds,	&molecule.bonds };
		renderer.addMolecule( proxyMolecule );
		*/

		// std::string data;
		// Network::httpRequestGet( "https://mmtf.rcsb.org/v1.0/full/4HHB", data );
		const std::string name = "4hhb.pdb";
		const FilePath	  path = Filesystem::getExecutableDir() / name;

		// Read model file.
		Reader::Molecule			reader;
		VTX::Core::Struct::Molecule molecule;
		reader.readFile( path, molecule );
		// reader.readBuffer( data, "4hhb.mmtf", molecule );

		VTX::IO::Util::SecondaryStructure::computeStride( molecule );

		// Proxify.
		// Move or maybe redo.

		size_t										   sizeAtoms  = molecule.trajectory.frames.front().size();
		std::vector<VTX::Core::ChemDB::Atom::SYMBOL> & symbols	  = molecule.atomSymbols;
		std::vector<uchar>							   colorAtoms = std::vector<uchar>( sizeAtoms );
		std::generate(
			colorAtoms.begin(),
			colorAtoms.end(),
			[ &symbols ]
			{
				static int i = 0;
				return VTX::Core::ChemDB::Color::getColorIndex( symbols[ i++ ] );
			}
		);
		std::vector<float> radii( sizeAtoms );
		std::generate(
			radii.begin(),
			radii.end(),
			[ &symbols ]
			{
				static int i = 0;
				return VTX::Core::ChemDB::Atom::SYMBOL_VDW_RADIUS[ int( symbols[ i++ ] ) ];
			}
		);
		std::vector<bool> visibilities = std::vector<bool>( sizeAtoms, true );
		std::vector<bool> selections   = std::vector<bool>( sizeAtoms, false );
		std::vector<uint> idAtoms( sizeAtoms );
		std::iota( idAtoms.begin(), idAtoms.end(), 0 );
		std::vector<uint> bondsIndex( molecule.bondPairAtomIndexes.size() );

		size_t			  sizeResidue = molecule.residueOriginalIds.size();
		std::vector<uint> idResidues( sizeResidue );
		std::iota( idResidues.begin(), idResidues.end(), 0 );
		std::vector<uchar> colorAResidues = std::vector<uchar>( sizeResidue );
		std::generate( colorAResidues.begin(), colorAResidues.end(), [] { return rand() % 256; } );

		std::vector<uchar> sstypes( sizeResidue, 0 );

		Renderer::StructProxyMolecule proxyMolecule
			= { &molecule.transform,
				&molecule.trajectory.frames.front(),
				&colorAtoms,
				&radii,
				&visibilities,
				&selections,
				&idAtoms,
				&molecule.bondPairAtomIndexes,
				&molecule.atomNames,
				&idResidues,
				reinterpret_cast<std::vector<uchar> *>( &molecule.residueSecondaryStructureTypes ),
				&colorAResidues,
				&molecule.residueFirstAtomIndexes,
				&molecule.residueAtomCounts,
				&molecule.chainFirstResidues,
				&molecule.chainResidueCounts };

		renderer.addMolecule( proxyMolecule );

		renderer.build();

		renderer.setColorLayout( VTX::Core::ChemDB::Color::COLOR_LAYOUT_JMOL );

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
