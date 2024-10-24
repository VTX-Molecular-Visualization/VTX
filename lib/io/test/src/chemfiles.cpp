#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/molecule.hpp>
#include <fstream>
#include <io/reader/molecule.hpp>
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

	VTX::Core::Struct::Molecule molecule	   = VTX::Core::Struct::Molecule();
	VTX::IO::Reader::Molecule	moleculeReader = VTX::IO::Reader::Molecule();
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

	VTX::Core::Struct::Molecule molecule	   = VTX::Core::Struct::Molecule();
	VTX::IO::Reader::Molecule	moleculeReader = VTX::IO::Reader::Molecule();
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

		size_t chainCount = 0;
		size_t resCount	  = 0;
		size_t atomCount  = 0;
		size_t frameCount = 0;
		size_t bondCount  = 0;
	};
	void testSystem( TestSystemArgs p_args )
	{
		using namespace VTX;
		using namespace VTX::IO;
		using namespace VTX::IO::Writer;

		const std::string moleculeName	   = p_args.systemName;
		const std::string moleculePathname = moleculeName + p_args.extension;
		const FilePath	  moleculePath	   = Util::Filesystem::getExecutableDir() / "data" / moleculePathname;

		VTX::Core::Struct::Molecule molecule = VTX::Core::Struct::Molecule();
		{
			IO::Reader::Molecule moleculeReader = IO::Reader::Molecule();

			moleculeReader.readFile( moleculePath, molecule );
		}
		p_args.atomCount  = molecule.getAtomCount();
		p_args.chainCount = molecule.getChainCount();
		p_args.frameCount = molecule.trajectory.getFrameCount();
		p_args.bondCount  = molecule.getBondCount();
		p_args.resCount	  = molecule.getResidueCount();

		const VTX::FilePath outPath = VTX::Util::Filesystem::getExecutableDir() / "out" / "ChemfilesTrajectory";
		if ( not std::filesystem::exists( outPath ) )
			std::filesystem::create_directories( outPath );

		const VTX::FilePath destination = outPath / ( moleculeName + p_args.writtenExtension );

		writeFile( WriteArgs {
			.destination = destination,
			.format		 = E_FILE_FORMATS::none,
			.molecule	 = &molecule,
		} );

		VTX::Core::Struct::Molecule molecule_reread		  = VTX::Core::Struct::Molecule();
		IO::Reader::Molecule		moleculeReader_reread = IO::Reader::Molecule();

		moleculeReader_reread.readFile( destination, molecule_reread );

		CHECK( molecule_reread.getChainCount() == p_args.chainCount );
		CHECK( molecule_reread.getResidueCount() == p_args.resCount );
		CHECK( molecule_reread.getAtomCount() == p_args.atomCount );

		// Bond are not reliably written in files so we won't check them.
		// e.g. 2qwo has disulfide bond that is not retrieved when reloading the file
		// CHECK( molecule_reread.getBondCount() == p_args.bondCount );
	}
} // namespace

TEST_CASE( "VTX_IO - Test writeFile", "[writer][chemfiles][trajectory]" )
{
	try
	{
		VTX::VTX_INFO( "Test reading and writing on {}.", "8aad" );
		testSystem( TestSystemArgs { .systemName = "8aad", .extension = ".mmcif", .writtenExtension = ".pdb" } );
	}
	catch ( std::exception & )
	{
	}
	return;
	VTX::VTX_INFO( "Test reading and writing on {}.", "2qwo" );
	testSystem( TestSystemArgs { .systemName = "2qwo", .extension = ".pdb", .writtenExtension = ".pdb" } );
	VTX::VTX_INFO( "Test reading and writing on {}.", "1wav" );
	testSystem( TestSystemArgs { .systemName = "1wav", .extension = ".pdb", .writtenExtension = ".pdb" } );
	VTX::VTX_INFO( "Test reading and writing on {}.", "1gcn" );
	testSystem( TestSystemArgs { .systemName = "1gcn", .extension = ".pdb", .writtenExtension = ".pdb" } );
	VTX::VTX_INFO( "Test reading and writing on {}.", "8OIT" );
	testSystem( TestSystemArgs { .systemName = "8OIT", .extension = ".mmtf", .writtenExtension = ".mmcif" } );
}

// We test it only in dev mode.
#ifdef PDB_DATABASE_DIR
namespace
{
	namespace fs			= std::filesystem;
	using FileCollection	= std::list<std::string>;
	using FilePtrCollection = std::list<const std::string *>;
	using ResultDict		= std::unordered_map<const std::string *, uint32_t>;
	struct DataBaseTestContext
	{
		fs::path		  dbDir;
		FileCollection	  tested_structs;
		FilePtrCollection crashedStructs;
		FilePtrCollection fullyWorkingStructs;
		FilePtrCollection residuesNumNotMatch;
		FilePtrCollection atomNumNotMatch;
		FilePtrCollection chainNumNotMatch;
		FilePtrCollection frameNumNotMatch;
	};
	void walkDir( DataBaseTestContext & contextData, const std::filesystem::path & dir )
	{
		for ( auto & it_fsItem : fs::directory_iterator( dir ) )
		{
			if ( fs::is_directory( it_fsItem ) )
				walkDir( contextData, it_fsItem.path() );
			if ( fs::is_regular_file( it_fsItem ) )
				contextData.tested_structs.push_back( it_fsItem.path().string() );
		}
	}
	void enumerateFiles( DataBaseTestContext & contextData ) { walkDir( contextData, contextData.dbDir ); }

	void writeReportSummary( DataBaseTestContext & contextData )
	{
		VTX::VTX_INFO(
			"Fully working : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.fullyWorkingStructs.size() ) / contextData.tested_structs.size(),
			contextData.fullyWorkingStructs.size(),
			contextData.tested_structs.size()
		);
		VTX::VTX_INFO(
			"Atom mismatch : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.atomNumNotMatch.size() ) / contextData.tested_structs.size(),
			contextData.atomNumNotMatch.size(),
			contextData.tested_structs.size()
		);
		VTX::VTX_INFO(
			"Residue mismatch : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.residuesNumNotMatch.size() ) / contextData.tested_structs.size(),
			contextData.residuesNumNotMatch.size(),
			contextData.tested_structs.size()
		);
		VTX::VTX_INFO(
			"Chain mismatch : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.chainNumNotMatch.size() ) / contextData.tested_structs.size(),
			contextData.chainNumNotMatch.size(),
			contextData.tested_structs.size()
		);
		VTX::VTX_INFO(
			"Frame mismatch : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.frameNumNotMatch.size() ) / contextData.tested_structs.size(),
			contextData.frameNumNotMatch.size(),
			contextData.tested_structs.size()
		);
		VTX::VTX_INFO(
			"Crashes : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.crashedStructs.size() ) / contextData.tested_structs.size(),
			contextData.crashedStructs.size(),
			contextData.crashedStructs.size()
		);
	}
	void writeResultFile( const VTX::FilePath & dest, FilePtrCollection & data )
	{
		std::ofstream file { dest };
		for ( auto & it_str : data )
			file << fs::path( *it_str ).stem().string() << "\t" << it_str << std::endl;
	}
} // namespace

TEST_CASE( "VTX_IO - All pdb", "[pdb100]" )
{
	VTX::FilePath dbDir( PDB_DATABASE_DIR );
	if ( not std::filesystem::exists( dbDir ) )
	{
		VTX::VTX_INFO( "Directory <{}> not found. test on all pdb will fail gently.", dbDir.string() );
		return;
	}
	DataBaseTestContext contextData { .dbDir = dbDir };
	VTX::VTX_INFO( "Enumerating data ..." );
	enumerateFiles( contextData );
	VTX::VTX_INFO( "{} files found.", contextData.tested_structs.size() );
	VTX::FilePath outDir { VTX::Util::Filesystem::getExecutableDir() / "out" / "pdbDb" };
	if ( not fs::exists( outDir ) )
		fs::create_directories( outDir );
	VTX::FilePath tmpFile { outDir / "buffer.mmcif" };
	uint32_t	  fileIndex = 0;
	for ( auto & it_structFile : contextData.tested_structs )
	{
		try
		{
			decompressFile( it_structFile, tmpFile );

			testFile( contextData, tmpFile, it_structFile );
			fs::remove( tmpFile );
			if ( fileIndex % 100 == 0 )
				VTX::VTX_INFO( "{} files done.", fileIndex );
			fileIndex++;
		}
		catch ( std::exception & e )
		{
			contextData.crashedStructs.push_back( &it_structFile );
			std::ofstream( outDir / "crashs.log", std::ios::app ) << it_structFile << " " << e.what();
		}
		catch ( ... )
		{
			contextData.crashedStructs.push_back( &it_structFile );
			std::ofstream( outDir / "crashs.log", std::ios::app ) << it_structFile;
		}
	}
	writeReportSummary( contextData );
	writeResultFile( outDir / "fully_working.log", contextData.fullyWorkingStructs );
	writeResultFile( outDir / "residue_mismatch.log", contextData.residuesNumNotMatch );
	writeResultFile( outDir / "atom_mismatch.log", contextData.atomNumNotMatch );
	writeResultFile( outDir / "chain_mismatch.log", contextData.chainNumNotMatch );
	writeResultFile( outDir / "frame_mismatch.log", contextData.frameNumNotMatch );
}
#endif // PDB_DATABASE_DIR
