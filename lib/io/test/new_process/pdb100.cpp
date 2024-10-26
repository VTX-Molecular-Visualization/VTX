#include "test/process/shared.hpp"
#include <algorithm>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <core/struct/molecule.hpp>
#include <fstream>
#include <util/chrono.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>

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
			boost::interprocess::shared_memory_object::remove( VTX::IO::test::SHM_REREADRSLT_MAP_SEGNAME );
			boost::interprocess::named_mutex::remove( VTX::IO::test::SHM_REREADRSLT_MUTEX );
		}
		~shm_remove()
		{
			boost::interprocess::shared_memory_object::remove( VTX::IO::test::SHM_FILESTR_DEQUE_SEGNAME );
			boost::interprocess::named_mutex::remove( VTX::IO::test::SHM_FILESTR_MUTEX );
			boost::interprocess::shared_memory_object::remove( VTX::IO::test::SHM_REREADRSLT_MAP_SEGNAME );
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
			static_cast<double>( contextData.num_fullyworking ) / contextData.tested_structs.size() * 100.,
			contextData.num_fullyworking,
			contextData.tested_structs.size()
		);
		VTX::VTX_INFO(
			"Atom mismatch : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.num_atomNumNotMatch ) / contextData.tested_structs.size() * 100.,
			contextData.num_atomNumNotMatch,
			contextData.tested_structs.size()
		);
		VTX::VTX_INFO(
			"Residue mismatch : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.num_residuesNumNotMatch ) / contextData.tested_structs.size() * 100.,
			contextData.num_residuesNumNotMatch,
			contextData.tested_structs.size()
		);
		VTX::VTX_INFO(
			"Chain mismatch : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.num_chainNumNotMatch ) / contextData.tested_structs.size() * 100.,
			contextData.num_chainNumNotMatch,
			contextData.tested_structs.size()
		);
		VTX::VTX_INFO(
			"Frame mismatch : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.num_frameNumNotMatch ) / contextData.tested_structs.size() * 100.,
			contextData.num_frameNumNotMatch,
			contextData.tested_structs.size()
		);
		VTX::VTX_INFO(
			"Crashes : {:0.02f}% ({}/{})",
			static_cast<double>( contextData.num_crashed ) / contextData.tested_structs.size() * 100.,
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
			if ( static_cast<bool>( it_strRslt.second & result_type )
				 || ( result_type == VTX::IO::test::RereadResult::fully_working
					  && it_strRslt.second == VTX::IO::test::RereadResult::fully_working ) )
				file << fs::path( *it_strRslt.first ).stem().string() << "\t" << *it_strRslt.first << std::endl;
		}
	}

	class RestartableWorker // We expect workers to crash eventually, so we need to restart them sometimes
	{
		struct _Data
		{
			const char * exeStr		 = "";
			int			 num		 = 0;
			bool		 finished	 = false;
			int			 thr_retcode = 1;
			std::string	 startStr	 = std::format( "{} {}", exeStr, num );
			std::jthread thr { [ & ]()
							   {
								   while ( this->thr_retcode != 0 )
								   {
									   VTX::VTX_INFO( "Starting worker {}.", this->num, this->thr_retcode );
									   this->thr_retcode = system( startStr.data() );
									   VTX::VTX_INFO(
										   "Worker {} terminated with return code {}.", this->num, this->thr_retcode
									   );
								   }
								   VTX::VTX_INFO( "Worker {} won't restart.", this->num );
								   this->finished = true;
							   } };
		};
		std::unique_ptr<_Data> _data = nullptr;

	  public:
		RestartableWorker() = delete;
		RestartableWorker( const char * startStr, int num ) : _data( std::make_unique<_Data>( startStr, num ) ) {}
		bool finished() const { return _data->finished; }
	};
	void _constructAndFillDeque( DataBaseTestContext & contextData )
	{
		using namespace VTX::IO::test;

		const size_t size_shm_deque
			= contextData.tested_structs.size()
				  * ( contextData.tested_structs.begin()->size() + sizeof( String ) + 50 /* deque memory overhead ? */ )
			  + 500;

		boost::interprocess::managed_shared_memory sharedSegment(
			boost::interprocess::create_only, VTX::IO::test::SHM_FILESTR_DEQUE_SEGNAME, size_shm_deque
		);
		StringDequeAllocator dequeAlloc( sharedSegment.get_segment_manager() );
		StringAllocator		 strAlloc( sharedSegment.get_segment_manager() );
		StringDeque * fileStrDeque = sharedSegment.construct<StringDeque>( SHM_FILESTR_DEQUE_OBJNAME )( dequeAlloc );

		for ( auto & it_filepathStr : contextData.tested_structs )
		{
			fileStrDeque->emplace_back( strAlloc ); // segfault ?
			fileStrDeque->back().assign( it_filepathStr.begin(), it_filepathStr.end() );
		}
	}
	void _constructMap( DataBaseTestContext & contextData )
	{
		using namespace VTX::IO::test;
		const size_t size_shm_map
			= contextData.tested_structs.size() * ( 50 + sizeof( RereadResult ) + sizeof( uint64_t ) ) + 500;

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
	void promptJobStatus( DataBaseTestContext & contextData )
	{
		using namespace boost::interprocess;
		using namespace VTX::IO::test;
		size_t mapSize = 0;
		{
			named_mutex									  mutex( open_or_create, SHM_REREADRSLT_MUTEX );
			scoped_lock<boost::interprocess::named_mutex> lock( mutex );
			boost::interprocess::managed_shared_memory	  sharedSegment(
				   boost::interprocess::open_only, VTX::IO::test::SHM_REREADRSLT_MAP_SEGNAME
			   );
			auto rsltMapPair = sharedSegment.find<RereadResultMap>( SHM_REREADRSLT_MAP_OBJNAME );
			if ( rsltMapPair.first == nullptr )
				throw;
			mapSize = rsltMapPair.first->size();
		}
		VTX::VTX_INFO(
			"Job status : {:0.2f}% completion ({}/{})",
			100. * static_cast<double>( mapSize ) / static_cast<double>( contextData.tested_structs.size() ),
			mapSize,
			contextData.tested_structs.size()
		);
	}
	void testFiles( DataBaseTestContext & contextData )
	{
		using namespace VTX::IO::test;
		VTX::VTX_INFO( "Constructing Deque ..." );
		_constructAndFillDeque( contextData );

		VTX::VTX_INFO( "Constructing Map ..." );
		_constructMap( contextData );

		const int					   NUM_WORKER = 24;
		const char *				   startStr	  = "vtx_io_new_process";
		std::vector<RestartableWorker> workerPool;
		VTX::VTX_INFO( "Creating {} workers ...", NUM_WORKER );
		int workerIdx = 0;
		while ( workerIdx < NUM_WORKER )
		{
			workerPool.emplace_back( startStr, workerIdx++ );
		}

		uint64_t waitCount = 1;
		while ( not std::all_of(
			workerPool.begin(), workerPool.end(), []( const RestartableWorker & w ) { return w.finished(); }
		) )
		{
			std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
			if ( waitCount % 30 == 0 )
				promptJobStatus( contextData );
			if ( waitCount % 120 == 0 )
				workerPool.emplace_back( startStr, workerIdx++ );
			waitCount++;
		}
	}
	class Logger
	{
		DataBaseTestContext * _d;

	  public:
		Logger( DataBaseTestContext & d ) : _d( &d ) {}
		~Logger()
		{
			_exploitMap( *_d );
			VTX::FilePath outDir { VTX::Util::Filesystem::getExecutableDir() / "out" / "pdbDb" };
			if ( not fs::exists( outDir ) )
				fs::create_directories( outDir );
			writeReportSummary( *_d );
			writeResultFile(
				outDir / "fully_working.log", _d->testResults, VTX::IO::test::RereadResult::fully_working
			);
			writeResultFile( outDir / "crashed.log", _d->testResults, VTX::IO::test::RereadResult::crashed );
			writeResultFile(
				outDir / "residue_mismatch.log", _d->testResults, VTX::IO::test::RereadResult::residue_mismatch
			);
			writeResultFile(
				outDir / "atom_mismatch.log", _d->testResults, VTX::IO::test::RereadResult::atom_mismatch
			);
			writeResultFile(
				outDir / "chain_mismatch.log", _d->testResults, VTX::IO::test::RereadResult::chain_mismatch
			);
			writeResultFile(
				outDir / "frame_mismatch.log", _d->testResults, VTX::IO::test::RereadResult::frame_mismatch
			);
		}
	};
} // namespace

int main( int argc, char * argv[] )
{
	VTX::FilePath dbDir( PDB_DATABASE_DIR );
	if ( not std::filesystem::exists( dbDir ) )
	{
		VTX::VTX_INFO( "Directory <{}> not found. test on all pdb will fail gently.", dbDir.string() );
		return 0;
	}
	DataBaseTestContext contextData { .dbDir = dbDir };
	VTX::Util::Chrono	chrono;
	chrono.start();

	VTX::VTX_INFO( "Enumerating data ..." );
	enumerateFiles( contextData );
	VTX::VTX_INFO( "{} files found.", contextData.tested_structs.size() );

	Logger l( contextData );
	testFiles( contextData );

	chrono.stop();
	VTX::VTX_INFO( "Job duration : {}", VTX::Util::String::durationToStr( chrono.elapsedTime() ) );
	return 0;
}
#endif // PDB_DATABASE_DIR
