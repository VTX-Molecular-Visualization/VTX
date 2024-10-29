#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/system.hpp>
#include <fstream>
#include <io/reader/system.hpp>
#include <io/writer/chemfiles.hpp>
#include <io/writer/molecule.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <vector>

namespace
{
	using namespace VTX::IO::Writer;
	void twoWaterMolecules1frame( ChemfilesTrajectory & trajWriter )
	{
		System system = trajWriter.system();
		Frame  frame  = system.newFrame();
		Chain  chain  = system.newChain();
		chain.setId( "A" );
		chain.setName( "A" );
		Residue res = system.newResidue();
		chain.add( res );
		res.setResId( 1 );
		res.setSymbol( "SOL" );

		uint64_t i = 0;
		AtomId	 id_wat1_O { i++ };
		Atom	 wat1_O = system.newAtom( id_wat1_O );
		wat1_O.setName( "OW" );
		wat1_O.setSymbol( "O" );
		res.add( wat1_O );
		frame.set( wat1_O, { 42.019, 41.914, 16.013 } );

		AtomId id_wat1_H1 { i++ };
		Atom   wat1_H1 = system.newAtom( id_wat1_H1 );
		wat1_H1.setName( "HW1" );
		wat1_H1.setSymbol( "H" );
		res.add( wat1_H1 );
		frame.set( wat1_H1, { 41.159, 41.984, 15.603 } );

		AtomId id_wat1_H2 { i++ };
		Atom   wat1_H2 = system.newAtom( id_wat1_H2 );
		wat1_H2.setName( "HW2" );
		wat1_H2.setSymbol( "H" );
		res.add( wat1_H2 );
		frame.set( wat1_H2, { 42.339, 42.814, 16.063 } );

		res = system.newResidue();
		res.setResId( 2 );
		res.setSymbol( "SOL" );
		chain.add( res );

		AtomId id_wat2_O { i++ };
		Atom   wat2_O = system.newAtom( id_wat2_O );
		wat2_O.setName( "OW" );
		wat2_O.setSymbol( "O" );
		res.add( wat2_O );
		frame.set( wat2_O, { 36.731, 18.181, 36.460 } );

		AtomId id_wat2_H1 { i++ };
		Atom   wat2_H1 = system.newAtom( id_wat2_H1 );
		wat2_H1.setName( "HW1" );
		wat2_H1.setSymbol( "H" );
		res.add( wat2_H1 );
		frame.set( wat2_H1, { 35.841, 17.911, 36.230 } );

		AtomId id_wat2_H2 { i++ };
		Atom   wat2_H2 = system.newAtom( id_wat2_H2 );
		wat2_H2.setName( "HW2" );
		wat2_H2.setSymbol( "H" );
		res.add( wat2_H2 );
		frame.set( wat2_H2, { 36.621, 18.721, 37.240 } );

		system.bind( id_wat1_O, id_wat1_H1, E_BOND_ORDER::single );
		system.bind( id_wat1_O, id_wat1_H2, E_BOND_ORDER::single );
		system.bind( id_wat2_O, id_wat2_H1, E_BOND_ORDER::single );
		system.bind( id_wat2_O, id_wat2_H2, E_BOND_ORDER::single );
	}
	void twoWaterMolecules2frame( ChemfilesTrajectory & trajWriter )
	{
		System system = trajWriter.system();
		Frame  frame1 = system.newFrame();
		Frame  frame2 = system.newFrame();
		Chain  chain  = system.newChain();
		chain.setId( "A" );
		chain.setName( "A" );
		Residue res = system.newResidue();
		chain.add( res );
		res.setResId( 1 );
		res.setSymbol( "SOL" );

		uint64_t i = 0;
		AtomId	 id_wat1_O { i++ };
		Atom	 wat1_O = system.newAtom( id_wat1_O );
		wat1_O.setName( "OW" );
		wat1_O.setSymbol( "O" );
		res.add( wat1_O );
		frame1.set( wat1_O, { 42.019, 41.914, 16.013 } );
		frame2.set( wat1_O, { 43.019, 41.914, 16.013 } );

		AtomId id_wat1_H1 { i++ };
		Atom   wat1_H1 = system.newAtom( id_wat1_H1 );
		wat1_H1.setName( "HW1" );
		wat1_H1.setSymbol( "H" );
		res.add( wat1_H1 );
		frame1.set( wat1_H1, { 41.159, 41.984, 15.603 } );
		frame2.set( wat1_H1, { 42.159, 41.984, 15.603 } );

		AtomId id_wat1_H2 { i++ };
		Atom   wat1_H2 = system.newAtom( id_wat1_H2 );
		wat1_H2.setName( "HW2" );
		wat1_H2.setSymbol( "H" );
		res.add( wat1_H2 );
		frame1.set( wat1_H2, { 42.339, 42.814, 16.063 } );
		frame2.set( wat1_H2, { 43.339, 42.814, 16.063 } );

		res = system.newResidue();
		res.setResId( 2 );
		res.setSymbol( "SOL" );
		chain.add( res );

		AtomId id_wat2_O { i++ };
		Atom   wat2_O = system.newAtom( id_wat2_O );
		wat2_O.setName( "OW" );
		wat2_O.setSymbol( "O" );
		res.add( wat2_O );
		frame1.set( wat2_O, { 36.731, 18.181, 36.460 } );
		frame2.set( wat2_O, { 37.731, 18.181, 36.460 } );

		AtomId id_wat2_H1 { i++ };
		Atom   wat2_H1 = system.newAtom( id_wat2_H1 );
		wat2_H1.setName( "HW1" );
		wat2_H1.setSymbol( "H" );
		res.add( wat2_H1 );
		frame1.set( wat2_H1, { 35.841, 17.911, 36.230 } );
		frame2.set( wat2_H1, { 36.841, 17.911, 36.230 } );

		AtomId id_wat2_H2 { i++ };
		Atom   wat2_H2 = system.newAtom( id_wat2_H2 );
		wat2_H2.setName( "HW2" );
		wat2_H2.setSymbol( "H" );
		res.add( wat2_H2 );
		frame1.set( wat2_H2, { 36.621, 18.721, 37.240 } );
		frame2.set( wat2_H2, { 37.621, 18.721, 37.240 } );

		system.bind( id_wat1_O, id_wat1_H1, E_BOND_ORDER::single );
		system.bind( id_wat1_O, id_wat1_H2, E_BOND_ORDER::single );
		system.bind( id_wat2_O, id_wat2_H1, E_BOND_ORDER::single );
		system.bind( id_wat2_O, id_wat2_H2, E_BOND_ORDER::single );
	}
} // namespace

TEST_CASE( "VTX_IO - Test ChemfilesTrajectory writer, 1 frame", "[writer][chemfiles][trajectory][1 frame]" )
{
	using namespace VTX::IO::Writer;

	const VTX::FilePath outPath = VTX::Util::Filesystem::getExecutableDir() / "out" / "ChemfilesTrajectory";
	if ( not std::filesystem::exists( outPath ) )
		std::filesystem::create_directories( outPath );

	const VTX::FilePath waterPath = outPath / "water.pdb";

	{
		ChemfilesTrajectory trajWriter;
		trajWriter.setWriteDestination( waterPath );
		trajWriter.setWriteFormat( E_FILE_FORMATS::pdb );
		twoWaterMolecules1frame( trajWriter );
	}

	VTX::Core::Struct::System molecule		 = VTX::Core::Struct::System();
	VTX::IO::Reader::System	  moleculeReader = VTX::IO::Reader::System();
	moleculeReader.readFile( waterPath, molecule );

	CHECK( molecule.getChainCount() == 1 );
	CHECK( molecule.getBondCount() == 4 );
	CHECK( molecule.getResidueCount() == 2 );
	CHECK( molecule.getAtomCount() == 6 );
	CHECK( molecule.trajectory.getFrameCount() == 1 );
}
TEST_CASE( "VTX_IO - Test ChemfilesTrajectory writer, 2 frames", "[writer][chemfiles][trajectory][2 frames]" )
{
	using namespace VTX::IO::Writer;

	const VTX::FilePath outPath = VTX::Util::Filesystem::getExecutableDir() / "out" / "ChemfilesTrajectory";
	if ( not std::filesystem::exists( outPath ) )
		std::filesystem::create_directories( outPath );

	const VTX::FilePath waterPath = outPath / "water_2frames.pdb";

	{
		ChemfilesTrajectory trajWriter;
		trajWriter.setWriteDestination( waterPath );
		trajWriter.setWriteFormat( E_FILE_FORMATS::pdb );
		twoWaterMolecules2frame( trajWriter );
	}

	VTX::Core::Struct::System molecule		 = VTX::Core::Struct::System();
	VTX::IO::Reader::System	  moleculeReader = VTX::IO::Reader::System();
	moleculeReader.readFile( waterPath, molecule );

	CHECK( molecule.getChainCount() == 1 );
	CHECK( molecule.getBondCount() == 4 );
	CHECK( molecule.getResidueCount() == 2 );
	CHECK( molecule.getAtomCount() == 6 );
	CHECK( molecule.trajectory.getFrameCount() == 2 );
}

namespace
{
	struct TestSystemArgs
	{
		const char * systemName;
		const char * extension;
		const char * writtenExtension;
	};
	void testSystem( TestSystemArgs p_args )
	{
		using namespace VTX;
		using namespace VTX::IO;
		using namespace VTX::IO::Writer;

		const std::string moleculeName	   = p_args.systemName;
		const std::string moleculePathname = moleculeName + p_args.extension;
		const FilePath	  moleculePath	   = Util::Filesystem::getExecutableDir() / "data" / moleculePathname;

		VTX::Core::Struct::System molecule = VTX::Core::Struct::System();
		{
			IO::Reader::System moleculeReader = IO::Reader::System();

			moleculeReader.readFile( moleculePath, molecule );
		}
		size_t atomCount  = molecule.getAtomCount();
		size_t chainCount = molecule.getChainCount();
		size_t frameCount = molecule.trajectory.getFrameCount();
		size_t bondCount  = molecule.getBondCount();
		size_t resCount	  = molecule.getResidueCount();

		const VTX::FilePath outPath = VTX::Util::Filesystem::getExecutableDir() / "out" / "ChemfilesTrajectory";
		if ( not std::filesystem::exists( outPath ) )
			std::filesystem::create_directories( outPath );

		const VTX::FilePath destination = outPath / ( moleculeName + p_args.writtenExtension );

		writeFile( WriteArgs {
			.destination = destination,
			.format		 = E_FILE_FORMATS::none,
			.molecule	 = &molecule,
		} );

		VTX::Core::Struct::System molecule_reread		= VTX::Core::Struct::System();
		IO::Reader::System		  moleculeReader_reread = IO::Reader::System();

		moleculeReader_reread.readFile( destination, molecule_reread );

		CHECK( molecule_reread.getChainCount() == chainCount );
		CHECK( molecule_reread.getResidueCount() == resCount );
		CHECK( molecule_reread.getAtomCount() == atomCount );

		// Bond are not reliably written in files so we won't check them.
		// e.g. 2qwo has disulfide bond that is not retrieved when reloading the file
		// CHECK( molecule_reread.getBondCount() == p_args.bondCount );
	}
} // namespace

TEST_CASE( "VTX_IO - Test writeFile", "[writer][chemfiles][trajectory][specific_file]" )
{
	VTX::VTX_INFO( "Test reading and writing on {}.", "1idx" );
	VTX::VTX_INFO( "This one has reported atom mismatch" );
	testSystem( TestSystemArgs { .systemName = "1idx", .extension = ".cif", .writtenExtension = ".mmcif" } );
	VTX::VTX_INFO( "Test reading and writing on {}.", "202d" );
	VTX::VTX_INFO( "This one has reported residue mismatch" );
	testSystem( TestSystemArgs { .systemName = "202d", .extension = ".cif", .writtenExtension = ".mmcif" } );
	return;
}
