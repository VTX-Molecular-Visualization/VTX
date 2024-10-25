#include "test/process/shared.hpp"
#include <algorithm>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/struct/molecule.hpp>
#include <fstream>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

// We test it only in dev mode.
#ifdef PDB_DATABASE_DIR
namespace
{

	struct shm_remove
	{
		shm_remove()
		{
			boost::interprocess::shared_memory_object::remove( VTX::IO::test::SHM_FILESTR_DEQUE_SEGNAME );
			boost::interprocess::named_mutex::remove( VTX::IO::test::SHM_FILESTR_MUTEX );
			boost::interprocess::shared_memory_object::remove( VTX::IO::test::SHM_REREADRSLT_MAP );
			boost::interprocess::named_mutex::remove( VTX::IO::test::SHM_REREADRSLT_MUTEX );
		}
		~shm_remove()
		{
			boost::interprocess::shared_memory_object::remove( VTX::IO::test::SHM_FILESTR_DEQUE_SEGNAME );
			boost::interprocess::named_mutex::remove( VTX::IO::test::SHM_FILESTR_MUTEX );
			boost::interprocess::shared_memory_object::remove( VTX::IO::test::SHM_REREADRSLT_MAP );
			boost::interprocess::named_mutex::remove( VTX::IO::test::SHM_REREADRSLT_MUTEX );
		}
	} g_shared_mem_remover;

	namespace fs			= std::filesystem;
	using FileCollection	= std::list<std::string>;
	using FilePtrCollection = std::list<const std::string *>;
	using ResultDict		= std::unordered_map<const std::string *, uint32_t>;
	using TestResults		= std::map<const std::string *, VTX::IO::test::RereadResult>;
	struct DataBaseTestContext
	{
		fs::path	   dbDir;
		FileCollection tested_structs;
		TestResults	   testResults;

		uint32_t num_fullyworking		 = 0;
		uint32_t num_residuesNumNotMatch = 0;
		uint32_t num_atomNumNotMatch	 = 0;
		uint32_t num_chainNumNotMatch	 = 0;
		uint32_t num_frameNumNotMatch	 = 0;
		uint32_t num_crashed			 = 0;
	};
	void walkDir( FileCollection & contextData, const std::filesystem::path & dir )
	{
		for ( auto & it_fsItem : fs::directory_iterator( dir ) )
		{
			if ( fs::is_directory( it_fsItem ) )
				walkDir( contextData, it_fsItem.path() );
			if ( fs::is_regular_file( it_fsItem ) )
				contextData.push_back( it_fsItem.path().string() );
		}
	}
	void enumerateFiles( DataBaseTestContext & contextData )
	{
		walkDir( contextData.tested_structs, contextData.dbDir );
	}
	void writeReportSummary( DataBaseTestContext & contextData )
	{
		VTX::VTX_INFO(
			"Fully working : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.num_fullyworking ) / contextData.tested_structs.size(),
			contextData.num_fullyworking,
			contextData.tested_structs.size()
		);
		VTX::VTX_INFO(
			"Atom mismatch : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.num_atomNumNotMatch ) / contextData.tested_structs.size(),
			contextData.num_atomNumNotMatch,
			contextData.tested_structs.size()
		);
		VTX::VTX_INFO(
			"Residue mismatch : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.num_residuesNumNotMatch ) / contextData.tested_structs.size(),
			contextData.num_residuesNumNotMatch,
			contextData.tested_structs.size()
		);
		VTX::VTX_INFO(
			"Chain mismatch : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.num_chainNumNotMatch ) / contextData.tested_structs.size(),
			contextData.num_chainNumNotMatch,
			contextData.tested_structs.size()
		);
		VTX::VTX_INFO(
			"Frame mismatch : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.num_frameNumNotMatch ) / contextData.tested_structs.size(),
			contextData.num_frameNumNotMatch,
			contextData.tested_structs.size()
		);
		VTX::VTX_INFO(
			"Crashes : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.num_crashed ) / contextData.tested_structs.size(),
			contextData.num_crashed,
			contextData.tested_structs.size()
		);
	}
	void writeResultFile(
		const VTX::FilePath &				dest,
		TestResults &						data,
		const VTX::IO::test::RereadResult & result_type
	)
	{
		std::ofstream file { dest };

		for ( auto & it_strRslt : data )
		{
			if ( static_cast<bool>( it_strRslt.second & result_type ) )
				file << fs::path( *it_strRslt.first ).stem().string() << "\t" << *it_strRslt.first << std::endl;
		}
	}

	class RestartableWorker // We expect workers to crash eventually, so we need to restart them sometimes
	{
		std::string	 _startStr;
		int			 _num		  = 0;
		int			 _thr_retcode = 1;
		std::jthread _thr;

		static void _start_job( const std::string & startStr, int & retCode ) { retCode = system( startStr.data() ); }

	  public:
		RestartableWorker( const char * startStr, int num ) :
			_startStr( std::format( "{} {}", startStr, num ) ), _num( num ),
			_thr(
				[ & ]()
				{
					while ( this->_thr_retcode != 0 )
						RestartableWorker::_start_job( this->_startStr, this->_thr_retcode );
				}
			)
		{
		}
		bool finished() const { return _thr_retcode == 0; }
	};
	void _constructAndFillDeque( DataBaseTestContext & contextData )
	{
		using namespace VTX::IO::test;

		const size_t size_shm_deque
			= contextData.tested_structs.size() * ( contextData.tested_structs.begin()->size() + 10 ) + 500;

		boost::interprocess::managed_shared_memory sharedSegment(
			boost::interprocess::create_only, VTX::IO::test::SHM_FILESTR_DEQUE_SEGNAME, size_shm_deque
		);
		StringDequeAllocator dequeAlloc( sharedSegment.get_segment_manager() );
		StringAllocator		 stringAlloc( sharedSegment.get_segment_manager() );
		StringDeque * fileStrDeque = sharedSegment.construct<StringDeque>( SHM_FILESTR_DEQUE_OBJNAME )( dequeAlloc );

		for ( auto & it_filepathStr : contextData.tested_structs )
			fileStrDeque->emplace_back( it_filepathStr.begin(), it_filepathStr.end() ); // segfault ?
	}
	void _constructMap( DataBaseTestContext & contextData )
	{
		using namespace VTX::IO::test;
		const size_t size_shm_map
			= contextData.tested_structs.size() * ( 5 + sizeof( RereadResult ) + sizeof( uint64_t ) ) + 500;

		boost::interprocess::managed_shared_memory sharedSegment(
			boost::interprocess::create_only, VTX::IO::test::SHM_REREADRSLT_MAP_SEGNAME, size_shm_map
		);
		RereadResultMapAllocator mapAlloc( sharedSegment.get_segment_manager() );
		RereadResultMap * rsltMap = sharedSegment.construct<RereadResultMap>( SHM_REREADRSLT_MAP_OBJNAME )( mapAlloc );
	}
	void _exploitMap( DataBaseTestContext & contextData )
	{
		using namespace VTX::IO::test;

		boost::interprocess::managed_shared_memory sharedSegment(
			boost::interprocess::open_only, VTX::IO::test::SHM_REREADRSLT_MAP_SEGNAME
		);
		auto rsltMapPair = sharedSegment.find<RereadResultMap>( SHM_REREADRSLT_MAP_OBJNAME );
		if ( rsltMapPair.first == nullptr )
		{
			VTX::VTX_ERROR( "Map Not found ????" );
			return;
		}
		RereadResultMap * rsltMap = rsltMapPair.first;

		for ( auto & it_filepathStr : contextData.tested_structs )
		{
			uint64_t key = std::hash<std::string>()( it_filepathStr );
			if ( not rsltMap->contains( key ) )
			{
				VTX::VTX_INFO( "Key not found for {}", it_filepathStr );
				continue;
			}
			RereadResult rslt = rsltMap->at( key );
			contextData.testResults.emplace( &it_filepathStr, rslt );
			if ( rslt == RereadResult::crashed )
			{
				contextData.num_crashed++;
				continue;
			}
			else if ( rslt == RereadResult::fully_working )
			{
				contextData.num_fullyworking++;
				continue;
			}

			contextData.num_atomNumNotMatch += static_cast<bool>( rslt & RereadResult::atom_mismatch );
			contextData.num_chainNumNotMatch += static_cast<bool>( rslt & RereadResult::chain_mismatch );
			contextData.num_residuesNumNotMatch += static_cast<bool>( rslt & RereadResult::residue_mismatch );
			contextData.num_frameNumNotMatch += static_cast<bool>( rslt & RereadResult::frame_mismatch );
		}
	}
	void testFiles( DataBaseTestContext & contextData )
	{
		using namespace VTX::IO::test;
		_constructAndFillDeque( contextData );
		_constructMap( contextData );

		const int					   NUM_WORKER = 10;
		const char *				   startStr	  = "vtx_io_new_process";
		std::vector<RestartableWorker> workerPool;
		for ( int i = 0; i < NUM_WORKER; i++ )
			workerPool.emplace_back( startStr, i );

		while ( std::all_of(
			workerPool.begin(), workerPool.end(), []( const RestartableWorker & w ) { return w.finished(); }
		) )
			std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

		_exploitMap( contextData );
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

	testFiles( contextData );

	VTX::FilePath outDir { VTX::Util::Filesystem::getExecutableDir() / "out" / "pdbDb" };
	if ( not fs::exists( outDir ) )
		fs::create_directories( outDir );
	writeReportSummary( contextData );
	writeResultFile(
		outDir / "fully_working.log", contextData.testResults, VTX::IO::test::RereadResult::fully_working
	);
	writeResultFile( outDir / "crashed.log", contextData.testResults, VTX::IO::test::RereadResult::crashed );
	writeResultFile(
		outDir / "residue_mismatch.log", contextData.testResults, VTX::IO::test::RereadResult::residue_mismatch
	);
	writeResultFile(
		outDir / "atom_mismatch.log", contextData.testResults, VTX::IO::test::RereadResult::atom_mismatch
	);
	writeResultFile(
		outDir / "chain_mismatch.log", contextData.testResults, VTX::IO::test::RereadResult::chain_mismatch
	);
	writeResultFile(
		outDir / "frame_mismatch.log", contextData.testResults, VTX::IO::test::RereadResult::frame_mismatch
	);
}
#endif // PDB_DATABASE_DIR
