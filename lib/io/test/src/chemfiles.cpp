#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_approx.hpp>
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
	VTX::IO::Metadata						metadata;
	VTX::Util::StopToken					t;
	VTX::IO::SystemReader					systemReader( waterPath, VTX::IO::READER_OPTION::ALL, t );
	VTX::IO::AtomPositions					positions;
	VTX::Core::ChemDB::Category::Dictionary dict = VTX::Core::ChemDB::Category::createDefaultDictionary();
	systemReader.get( dict, topology, metadata );
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
	VTX::IO::SystemReader					systemReader( waterPath, VTX::IO::READER_OPTION::ALL, t );
	VTX::Core::ChemDB::Category::Dictionary dict = VTX::Core::ChemDB::Category::createDefaultDictionary();
	VTX::IO::Metadata						metadata;
	systemReader.get( dict, topology, metadata );

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
			VTX::IO::SystemReader systemReader( systemPath, VTX::IO::READER_OPTION::ALL, t );
			VTX::IO::Metadata	  metadata;
			systemReader.get( dict, topology, metadata );
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

		{
			std::vector<WriteArgs::System> systems;
			systems.emplace_back( WriteArgs::System { &topology, std::move( traj ) } );
			writeFile(
				WriteArgs {
					.destination = destination,
					.format		 = E_FILE_FORMATS::none,
					.topologies	 = std::move( systems ),
				}
			);
		}

		VTX::Core::Struct::Topology system_reread;
		VTX::Util::StopToken		t;
		VTX::IO::SystemReader		systemReader( destination, VTX::IO::READER_OPTION::ALL, t );
		VTX::IO::Metadata			metadata;
		systemReader.get( dict, system_reread, metadata );

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
		// CHECK( results.matchBonds ); // Bonds won't be reliably written depending on the format. mmcif has
		// completly given up on bonds. bcif doesn't currently write struc_conn it seems. pdb is weird and I don't
		// know what's wrong but it is wrong. And so on
		CHECK( results.matchFrames );
		CHECK( results.firstRead.atLeastOneFrame );
		CHECK( results.reRead.atLeastOneFrame );
	}
}

// ─── Multi-system writeFile tests ─────────────────────────────────────────────

namespace
{
	// Single-frame trajectory whose positions are provided up front.
	struct FixedTrajectory
	{
		std::vector<VTX::Vec3f> frame;

		VTX::uint					frameCount() const { return frame.empty() ? 0u : 1u; }
		std::span<const VTX::Vec3f> getCurrentAtomPositions() const { return frame; }
		std::span<const VTX::Vec3f> getAtomPositions( const VTX::uint & ) const { return frame; }
	};

	// Builds the Topology fields that writeFile actually reads.
	// Usage: TopologyBuilder{}.chain("A").residue("SOL",1).atom("O",SYMBOL::A_O)....finish()
	struct TopologyBuilder
	{
		VTX::Core::Struct::Topology top;

		TopologyBuilder & chain( const std::string & name )
		{
			top.chainNames.push_back( name );
			top.chainFirstResidues.push_back( top.getResidueCount() );
			top.chainResidueCounts.push_back( 0 );
			return *this;
		}

		TopologyBuilder & residue( const std::string & name, int id )
		{
			top.residueSymbols.push_back( VTX::Core::ChemDB::Residue::SYMBOL::UNKNOWN );
			top.residueNames.push_back( name );
			top.residueChainIndexes.push_back( top.getChainCount() - 1 );
			top.residueOriginalIds.push_back( VTX::Index( id ) );
			top.residueFirstAtomIndexes.push_back( top.getAtomCount() );
			top.residueAtomCounts.push_back( 0 );
			top.residueFirstBondIndexes.push_back( 0 );
			top.residueBondCounts.push_back( 0 );
			top.residueSecondaryStructureTypes.push_back( VTX::Core::ChemDB::SecondaryStructure::TYPE::UNKNOWN );
			top.chainResidueCounts.back()++;
			return *this;
		}

		TopologyBuilder & atom( const std::string & name, VTX::Core::ChemDB::Atom::SYMBOL sym )
		{
			top.atomNames.push_back( name );
			top.atomSymbols.push_back( sym );
			top.atomResidueIndexes.push_back( top.getResidueCount() - 1 );
			top.residueAtomCounts.back()++;
			return *this;
		}

		VTX::Core::Struct::Topology finish() { return std::move( top ); }
	};

	VTX::FilePath multiOutDir()
	{
		VTX::FilePath path = VTX::Util::Filesystem::getExecutableDir() / "out" / "writeFile_multi";
		if ( !std::filesystem::exists( path ) )
			std::filesystem::create_directories( path );
		return path;
	}

	VTX::Core::Struct::Topology readBack(
		const VTX::FilePath &	 dest,
		VTX::IO::AtomPositions * positions	= nullptr,
		size_t *				 frameCount = nullptr
	)
	{
		VTX::Core::Struct::Topology				top;
		VTX::Util::StopToken					stop;
		VTX::IO::SystemReader					reader( dest, VTX::IO::READER_OPTION::ALL, stop );
		VTX::IO::Metadata						meta;
		VTX::Core::ChemDB::Category::Dictionary dict = VTX::Core::ChemDB::Category::createDefaultDictionary();
		reader.get( dict, top, meta );
		if ( positions )
			reader.get( *positions );
		if ( frameCount )
			*frameCount = reader.frameCount();
		return top;
	}
} // namespace

TEST_CASE(
	"VTX_IO - writeFile multi-system: all atoms from every system are present in output",
	"[writer][writeFile][multi-system]"
)
{
	using namespace VTX::IO::Writer;
	using namespace VTX::Core::ChemDB;

	// Water (3 atoms) in chain A and methane (5 atoms) in chain B
	auto water = TopologyBuilder {}
					 .chain( "A" )
					 .residue( "SOL", 1 )
					 .atom( "O", VTX::Core::ChemDB::Atom::SYMBOL::A_O )
					 .atom( "H1", VTX::Core::ChemDB::Atom::SYMBOL::A_H )
					 .atom( "H2", VTX::Core::ChemDB::Atom::SYMBOL::A_H )
					 .finish();

	auto methane = TopologyBuilder {}
					   .chain( "B" )
					   .residue( "MOL", 1 )
					   .atom( "C", VTX::Core::ChemDB::Atom::SYMBOL::A_C )
					   .atom( "H1", VTX::Core::ChemDB::Atom::SYMBOL::A_H )
					   .atom( "H2", VTX::Core::ChemDB::Atom::SYMBOL::A_H )
					   .atom( "H3", VTX::Core::ChemDB::Atom::SYMBOL::A_H )
					   .atom( "H4", VTX::Core::ChemDB::Atom::SYMBOL::A_H )
					   .finish();

	FixedTrajectory trajW, trajM;
	trajW.frame = { { 0.f, 0.f, 0.f }, { 0.5f, 1.f, 0.f }, { -0.5f, 1.f, 0.f } };
	trajM.frame = { { 5.f, 0.f, 0.f }, { 6.f, 0.f, 0.f }, { 4.f, 0.f, 0.f }, { 5.f, 1.f, 0.f }, { 5.f, -1.f, 0.f } };

	const VTX::FilePath			   dest = multiOutDir() / "multi_all_atoms.pdb";
	std::vector<WriteArgs::System> topologies;
	topologies.emplace_back( WriteArgs::System { &water, std::move( trajW ) } );
	topologies.emplace_back( WriteArgs::System { &methane, std::move( trajM ) } );
	writeFile(
		WriteArgs {
			.destination = dest,
			.format		 = E_FILE_FORMATS::pdb,
			.topologies	 = std::move( topologies ),
		}
	);

	size_t						fc;
	VTX::Core::Struct::Topology reread = readBack( dest, nullptr, &fc );

	CHECK( reread.getAtomCount() == 8 ); // 3 + 5
	CHECK( reread.getChainCount() == 2 );
	CHECK( reread.getResidueCount() == 2 );
	CHECK( fc == 1 );
}

TEST_CASE(
	"VTX_IO - writeFile multi-system: atoms with identical names across systems remain distinct",
	"[writer][writeFile][multi-system]"
)
{
	using namespace VTX::IO::Writer;
	using namespace VTX::Core::ChemDB;

	// Two water molecules: same atom names "O", "H1", "H2" in both systems
	auto water1 = TopologyBuilder {}
					  .chain( "A" )
					  .residue( "SOL", 1 )
					  .atom( "O", VTX::Core::ChemDB::Atom::SYMBOL::A_O )
					  .atom( "H1", VTX::Core::ChemDB::Atom::SYMBOL::A_H )
					  .atom( "H2", VTX::Core::ChemDB::Atom::SYMBOL::A_H )
					  .finish();

	auto water2 = TopologyBuilder {}
					  .chain( "B" )
					  .residue( "SOL", 2 )
					  .atom( "O", VTX::Core::ChemDB::Atom::SYMBOL::A_O )
					  .atom( "H1", VTX::Core::ChemDB::Atom::SYMBOL::A_H )
					  .atom( "H2", VTX::Core::ChemDB::Atom::SYMBOL::A_H )
					  .finish();

	FixedTrajectory traj1, traj2;
	traj1.frame = { { 0.f, 0.f, 0.f }, { 0.5f, 1.f, 0.f }, { -0.5f, 1.f, 0.f } };
	traj2.frame = { { 5.f, 0.f, 0.f }, { 5.5f, 1.f, 0.f }, { 4.5f, 1.f, 0.f } };

	const VTX::FilePath			   dest = multiOutDir() / "multi_same_names.pdb";
	std::vector<WriteArgs::System> topologies;
	topologies.emplace_back( WriteArgs::System { &water1, std::move( traj1 ) } );
	topologies.emplace_back( WriteArgs::System { &water2, std::move( traj2 ) } );
	writeFile(
		WriteArgs {
			.destination = dest,
			.format		 = E_FILE_FORMATS::pdb,
			.topologies	 = std::move( topologies ),
		}
	);

	VTX::Core::Struct::Topology reread = readBack( dest );

	// A shared name must not cause one atom to overwrite another: all 6 must survive
	CHECK( reread.getAtomCount() == 6 );
}

TEST_CASE(
	"VTX_IO - writeFile multi-system: each atom receives coordinates from its own system",
	"[writer][writeFile][multi-system]"
)
{
	using namespace VTX::IO::Writer;
	using namespace VTX::Core::ChemDB;

	// Two 2-atom systems; every atom has a unique integer x so we can identify it
	// regardless of the order the file format stores them.
	auto sys1 = TopologyBuilder {}
					.chain( "A" )
					.residue( "R1", 1 )
					.atom( "C1", VTX::Core::ChemDB::Atom::SYMBOL::A_C )
					.atom( "C2", VTX::Core::ChemDB::Atom::SYMBOL::A_C )
					.finish();

	auto sys2 = TopologyBuilder {}
					.chain( "B" )
					.residue( "R2", 1 )
					.atom( "N1", VTX::Core::ChemDB::Atom::SYMBOL::A_N )
					.atom( "N2", VTX::Core::ChemDB::Atom::SYMBOL::A_N )
					.finish();

	FixedTrajectory traj1, traj2;
	traj1.frame = { { 1.f, 0.f, 0.f }, { 2.f, 0.f, 0.f } };
	traj2.frame = { { 3.f, 0.f, 0.f }, { 4.f, 0.f, 0.f } };

	const VTX::FilePath			   dest = multiOutDir() / "multi_coords.pdb";
	std::vector<WriteArgs::System> topologies;
	topologies.emplace_back( WriteArgs::System { &sys1, std::move( traj1 ) } );
	topologies.emplace_back( WriteArgs::System { &sys2, std::move( traj2 ) } );
	writeFile(
		WriteArgs {
			.destination = dest,
			.format		 = E_FILE_FORMATS::pdb,
			.topologies	 = std::move( topologies ),
		}
	);

	VTX::IO::AtomPositions		positions;
	VTX::Core::Struct::Topology reread = readBack( dest, &positions );

	REQUIRE( positions.size() == 4 );

	std::vector<float> xs;
	for ( const auto & p : positions )
		xs.push_back( p[ 0 ] );
	std::sort( xs.begin(), xs.end() );

	// Every original coordinate must survive unmodified
	CHECK( xs[ 0 ] == Catch::Approx( 1.f ).margin( 0.01f ) );
	CHECK( xs[ 1 ] == Catch::Approx( 2.f ).margin( 0.01f ) );
	CHECK( xs[ 2 ] == Catch::Approx( 3.f ).margin( 0.01f ) );
	CHECK( xs[ 3 ] == Catch::Approx( 4.f ).margin( 0.01f ) );
}

TEST_CASE(
	"VTX_IO - writeFile atom filter: excluded atoms absent, kept atoms retain correct coordinates",
	"[writer][writeFile][filter]"
)
{
	using namespace VTX::IO::Writer;
	using namespace VTX::Core::ChemDB;

	// 3-atom residue: keep C (idx 0) and O (idx 2), exclude N (idx 1)
	auto top = TopologyBuilder {}
				   .chain( "A" )
				   .residue( "RES", 1 )
				   .atom( "C", VTX::Core::ChemDB::Atom::SYMBOL::A_C ) // index 0 – kept
				   .atom( "N", VTX::Core::ChemDB::Atom::SYMBOL::A_N ) // index 1 – filtered out
				   .atom( "O", VTX::Core::ChemDB::Atom::SYMBOL::A_O ) // index 2 – kept
				   .finish();

	FixedTrajectory traj;
	traj.frame = { { 1.f, 0.f, 0.f }, { 99.f, 0.f, 0.f }, { 3.f, 0.f, 0.f } };

	AtomFilter keepNonN
		= []( const VTX::Core::Struct::Topology & t, const size_t & i ) -> bool { return t.atomNames[ i ] != "N"; };

	const VTX::FilePath			   dest = multiOutDir() / "filter_single.pdb";
	std::vector<WriteArgs::System> topologies;
	topologies.emplace_back( WriteArgs::System { &top, std::move( traj ), keepNonN } );
	writeFile(
		WriteArgs {
			.destination = dest,
			.format		 = E_FILE_FORMATS::pdb,
			.topologies	 = std::move( topologies ),
		}
	);

	VTX::IO::AtomPositions		positions;
	VTX::Core::Struct::Topology reread = readBack( dest, &positions );

	REQUIRE( reread.getAtomCount() == 2 );
	REQUIRE( positions.size() == 2 );

	// The sentinel coordinate 99 must not appear
	for ( const auto & p : positions )
		CHECK( p[ 0 ] != Catch::Approx( 99.f ).margin( 0.01f ) );

	// C (x=1) and O (x=3) must have their original coordinates
	std::vector<float> xs = { positions[ 0 ][ 0 ], positions[ 1 ][ 0 ] };
	std::sort( xs.begin(), xs.end() );
	CHECK( xs[ 0 ] == Catch::Approx( 1.f ).margin( 0.01f ) );
	CHECK( xs[ 1 ] == Catch::Approx( 3.f ).margin( 0.01f ) );
}

TEST_CASE(
	"VTX_IO - writeFile multi-system with filter: combined atom count correct, filtered atoms absent",
	"[writer][writeFile][multi-system][filter]"
)
{
	using namespace VTX::IO::Writer;
	using namespace VTX::Core::ChemDB;

	// System 1: 3 carbons, no filter
	auto sys1 = TopologyBuilder {}
					.chain( "A" )
					.residue( "R1", 1 )
					.atom( "C1", VTX::Core::ChemDB::Atom::SYMBOL::A_C )
					.atom( "C2", VTX::Core::ChemDB::Atom::SYMBOL::A_C )
					.atom( "C3", VTX::Core::ChemDB::Atom::SYMBOL::A_C )
					.finish();

	// System 2: O1 (kept), N (filtered), O2 (kept).
	// The filter is independent of positions: every topology atom has a position in the
	// frame, including N. fillFrames skips filtered atoms via the sentinel from getAtomIdx.
	auto sys2 = TopologyBuilder {}
					.chain( "B" )
					.residue( "R2", 2 )
					.atom( "O1", VTX::Core::ChemDB::Atom::SYMBOL::A_O ) // index 0 – kept
					.atom( "N", VTX::Core::ChemDB::Atom::SYMBOL::A_N )	// index 1 – filtered
					.atom( "O2", VTX::Core::ChemDB::Atom::SYMBOL::A_O ) // index 2 – kept
					.finish();

	AtomFilter keepNonN = []( const VTX::Core::Struct::Topology & t, const size_t & i ) -> bool
	{
		//
		return t.atomNames[ i ] != "N";
		//
	};

	FixedTrajectory traj1, traj2;
	traj1.frame = { { 1.f, 0.f, 0.f }, { 2.f, 0.f, 0.f }, { 3.f, 0.f, 0.f } };
	traj2.frame = { { 4.f, 0.f, 0.f }, { 99.f, 0.f, 0.f }, { 5.f, 0.f, 0.f } }; // one position per atom

	const VTX::FilePath			   dest = multiOutDir() / "filter_multi.pdb";
	std::vector<WriteArgs::System> topologies;
	topologies.emplace_back( WriteArgs::System { &sys1, std::move( traj1 ) } );
	topologies.emplace_back( WriteArgs::System { &sys2, std::move( traj2 ), keepNonN } );
	writeFile(
		WriteArgs {
			.destination = dest,
			.format		 = E_FILE_FORMATS::pdb,
			.topologies	 = std::move( topologies ),
		}
	);

	VTX::IO::AtomPositions		positions;
	VTX::Core::Struct::Topology reread = readBack( dest, &positions );

	// 3 (sys1) + 2 (sys2 minus filtered N) = 5
	CHECK( reread.getAtomCount() == 5 );
	CHECK( reread.getChainCount() == 2 );

	for ( VTX::Index i = 0; i < reread.getAtomCount(); ++i )
		CHECK( reread.atomNames[ i ] != "N" );

	REQUIRE( positions.size() == 5 );
	std::vector<float> xs;
	for ( const auto & p : positions )
		xs.push_back( p[ 0 ] );
	std::sort( xs.begin(), xs.end() );
	CHECK( xs[ 0 ] == Catch::Approx( 1.f ).margin( 0.01f ) );
	CHECK( xs[ 4 ] == Catch::Approx( 5.f ).margin( 0.01f ) );
}
