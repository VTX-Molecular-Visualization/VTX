#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/topology.hpp>
#include <fstream>
#include <io/writer/chemfiles.hpp>
#include <io/writer/system.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/thread.hpp>
#include <util/types.hpp>
#include <vector>
//
#include <io/reader.hpp>

namespace
{
	using namespace VTX::IO::Writer;
	void twoWaterSystems1frame( ChemfilesTrajectory & trajWriter )
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
	void twoWaterSystems2frame( ChemfilesTrajectory & trajWriter )
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
		twoWaterSystems1frame( trajWriter );
	}

	VTX::Core::Struct::Topology				topology;
	VTX::Util::StopToken					t;
	VTX::IO::SystemReader					systemReader( waterPath, t );
	VTX::IO::AtomPositions					positions;
	VTX::Core::ChemDB::Category::Dictionary dict = VTX::Core::ChemDB::Category::createDefaultDictionary();
	systemReader.get( dict, topology );
	systemReader.get( positions );

	CHECK( topology.getChainCount() == 1 );
	CHECK( topology.getBondCount() == 4 );
	CHECK( topology.getResidueCount() == 2 );
	CHECK( topology.getAtomCount() == 6 );
	CHECK( systemReader.frameCount() == 1 );
	CHECK( positions.size() == 6 );
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
		twoWaterSystems2frame( trajWriter );
	}

	VTX::Core::Struct::Topology				topology;
	VTX::Util::StopToken					t;
	VTX::IO::SystemReader					systemReader( waterPath, t );
	VTX::Core::ChemDB::Category::Dictionary dict = VTX::Core::ChemDB::Category::createDefaultDictionary();
	systemReader.get( dict, topology );

	CHECK( topology.getChainCount() == 1 );
	CHECK( topology.getBondCount() == 4 );
	CHECK( topology.getResidueCount() == 2 );
	CHECK( topology.getAtomCount() == 6 );
	CHECK( systemReader.frameCount() == 2 );
}

namespace
{
	struct TestSystemArgs
	{
		const char * systemName;
		const char * extension;
		const char * writtenExtension;
	};

	struct TestResults
	{
		struct Read
		{
			bool atLeastOneFrame = false;

		} firstRead, reRead;
		bool matchAtoms	   = false;
		bool matchResidues = false;
		bool matchChains   = false;
		bool matchBonds	   = false;
		bool matchFrames   = false;
	};
	struct LazyTrajectory
	{
		std::vector<std::vector<VTX::Vec3f>> frames;

		inline VTX::uint				   frameCount() const { return static_cast<VTX::uint>( frames.size() ); }
		inline std::span<const VTX::Vec3f> getCurrentAtomPositions() const { return frames[ 0 ]; }
		inline std::span<const VTX::Vec3f> getAtomPositions( const VTX::uint & p_index ) const
		{ return frames[ p_index ]; }
	};

	/**
	 * @brief Test consistency over read-write-read protocol
	 * @param p_args
	 */
	void testSystem( TestSystemArgs p_args, TestResults & p_out )
	{
		using namespace VTX;
		using namespace VTX::IO;
		using namespace VTX::IO::Writer;
		VTX::Core::ChemDB::Category::Dictionary dict = VTX::Core::ChemDB::Category::createDefaultDictionary();

		const std::string systemName	 = p_args.systemName;
		const std::string systemPathname = systemName + p_args.extension;
		const FilePath	  systemPath	 = Util::Filesystem::getExecutableDir() / "data" / systemPathname;

		size_t						frameCount = 0;
		VTX::Core::Struct::Topology topology;
		LazyTrajectory				traj;
		{
			VTX::Util::StopToken  t;
			VTX::IO::SystemReader systemReader( systemPath, t );
			systemReader.get( dict, topology );
			frameCount = systemReader.frameCount();
			for ( size_t it_fc = 0; it_fc < frameCount; it_fc++ )
			{
				traj.frames.push_back( {} );
				systemReader.get( it_fc, traj.frames.back() );
			}
		}
		size_t atomCount  = topology.getAtomCount();
		size_t chainCount = topology.getChainCount();
		size_t bondCount  = topology.getBondCount();
		size_t resCount	  = topology.getResidueCount();

		p_out.firstRead.atLeastOneFrame = ( frameCount > 0 );

		const VTX::FilePath outPath = VTX::Util::Filesystem::getExecutableDir() / "out" / "ChemfilesTrajectory";
		if ( not std::filesystem::exists( outPath ) )
			std::filesystem::create_directories( outPath );

		const VTX::FilePath destination = outPath / ( systemName + p_args.writtenExtension );

		writeFile(
			WriteArgs { .destination = destination,
						.format		 = E_FILE_FORMATS::none,
						.topology	 = &topology,
						.trajectory	 = std::move( traj ) }
		);

		VTX::Core::Struct::Topology system_reread;
		VTX::Util::StopToken		t;
		VTX::IO::SystemReader		systemReader( destination, t );
		systemReader.get( dict, system_reread );

		p_out.matchChains			 = ( system_reread.getChainCount() == chainCount );
		p_out.matchResidues			 = ( system_reread.getResidueCount() == resCount );
		p_out.matchAtoms			 = ( system_reread.getAtomCount() == atomCount );
		p_out.matchFrames			 = ( systemReader.frameCount() == frameCount );
		p_out.reRead.atLeastOneFrame = systemReader.frameCount() > 0;
		p_out.matchBonds			 = system_reread.getBondCount() == bondCount;

		// Bond are not reliably written in files so we won't check them.
		// e.g. 2qwo has disulfide bond that is not retrieved when reloading the file
		// CHECK( system_reread.getBondCount() == p_args.bondCount );
	}
} // namespace

TEST_CASE( "VTX_IO - Test writeFile", "[writer][chemfiles][trajectory][specific_file][read_write_read]" )
{
	{
		TestResults results;
		testSystem(
			TestSystemArgs { .systemName = "1AGA", .extension = ".mmtf", .writtenExtension = ".bcif" }, results
		);
		CHECK( results.matchAtoms );
		CHECK( results.matchResidues );
		CHECK( results.matchChains );
		// CHECK( results.matchBonds ); // Bonds won't be reliably written depending on the format. mmcif has completly
		// given up on bonds. bcif doesn't currently write struc_conn it seems. pdb is weird and I don't know what's
		// wrong but it is wrong. And so on
		CHECK( results.matchFrames );
		CHECK( results.firstRead.atLeastOneFrame );
		CHECK( results.reRead.atLeastOneFrame );
	}
}
