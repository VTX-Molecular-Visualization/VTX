
//
#include <boost/asio.hpp>
#include <boost/process/process.hpp>
//
#include <array>
//
#include <vtx/secondary_structure/shared.hpp>
#include <vtx/secondary_structure/shared/interprocess.hpp>
//
#include <thread>
#include <vtx/secondary_structure/process.hpp>

namespace pdb100
{
	namespace shm
	{
		void createFileStrCollection( Context & p_context )
		{
			auto contextData = p_context.pdb100_system.open();

			const size_t size_shm_deque = contextData->size() * ( contextData->back().size() + sizeof( String ) + 50 )
										  + sizeof( StringDeque ) + 1000;

			boost::interprocess::managed_shared_memory sharedSegment(
				boost::interprocess::create_only, pdb100::shm::filestrDeque::SEGNAME, size_shm_deque
			);
			StringDequeAllocator dequeAlloc( sharedSegment.get_segment_manager() );
			StringAllocator		 strAlloc( sharedSegment.get_segment_manager() );
			StringDeque *		 fileStrDeque
				= sharedSegment.construct<StringDeque>( pdb100::shm::filestrDeque::OBJNAME )( dequeAlloc );

			for ( auto & it_filepathStr : *contextData )
			{
				fileStrDeque->emplace_back( strAlloc ); // segfault ?
				fileStrDeque->back().assign( it_filepathStr.begin(), it_filepathStr.end() );
			}
		}
		void createLivingProofCollection( Context & p_context )
		{
			const size_t shm_size = NUM_PROCESSES * ( sizeof( uint64_t ) * 2 ) + sizeof( LivingProofMap );

			boost::interprocess::managed_shared_memory sharedSegment(
				boost::interprocess::create_only, pdb100::shm::livingProof::SEGNAME, shm_size
			);
			LivingProofMapAllocator mapAllocator( sharedSegment.get_segment_manager() );
			LivingProofMap *		map
				= sharedSegment.construct<LivingProofMap>( pdb100::shm::livingProof::OBJNAME )( mapAllocator );
			map->allocate( NUM_PROCESSES );
		}
		void createResultMap( Context & p_context )
		{
			auto		 contextData = p_context.pdb100_system.open();
			const size_t shm_size	 = contextData->size() * ( contextData->back().size() + sizeof( String ) + 50 )
									+ sizeof( ResultFlagMap ) + 1000;
			boost::interprocess::managed_shared_memory sharedSegment(
				boost::interprocess::create_only, pdb100::shm::rsltMap::SEGNAME, shm_size
			);
			ResultFlagMapAllocator mapAllocator( sharedSegment.get_segment_manager() );
			ResultFlagMap *		   map
				= sharedSegment.construct<ResultFlagMap>( pdb100::shm::rsltMap::OBJNAME )( mapAllocator );
			map->allocate( contextData->size() );
		}
	} // namespace shm
	namespace
	{
		class RestartingProcess
		{
		  public:
			RestartingProcess( boost::asio::io_context & p_ctx ) : _proc( p_ctx.get_executor(), CHILD_PROCESS_NAME, {} )
			{
			}

			using ID = boost::interprocess::ipcdetail::OS_process_id_t;

			ID	 getId() { return _proc.id(); }
			bool finished() { return true; }
			void kill() { _proc.terminate(); }

		  private:
			boost::process::process _proc; // TODO
		};

		template<size_t SIZE>
		void restartCrashedProcess( std::array<RestartingProcess, SIZE> & p_processes )
		{
		}
		/**
		 * @brief Look at processes timestamp to detect processes that crashed and restart it if needed.
		 * @tparam SIZE
		 * @param p_processes
		 */
		template<size_t SIZE>
		void watchProcesses( std ::array<RestartingProcess, SIZE> & p_processes )
		{
			while ( true )
			{
				std::this_thread::sleep_for( std::chrono::seconds( 3 ) );
				restartCrashedProcess( p_processes );
				bool finished = true;
				for ( auto & it_process : p_processes )
				{
					finished &= it_process.finished();
				}
				if ( finished )
					break;
			}
		}
		template<typename T>
		RestartingProcess _generate( boost::asio::io_context & p_arg, T )
		{
			return { p_arg };
		}
		template<std::size_t... array_pack>
		std::array<RestartingProcess, NUM_PROCESSES> _generate(
			boost::asio::io_context & p_arg,
			std::index_sequence<array_pack...>
		)
		{
			return { _generate( p_arg, array_pack )... };
		}

		std::array<RestartingProcess, NUM_PROCESSES> generate( boost::asio::io_context & p_arg )
		{
			return _generate( p_arg, std::make_index_sequence<NUM_PROCESSES> {} );
		}

	} // namespace

	void testSystems( Context & p_context ) noexcept
	{
		shm::createFileStrCollection( p_context );
		shm::createLivingProofCollection( p_context );
		shm::createResultMap( p_context );

		boost::asio::io_context io_context;
		auto					processes = generate( io_context );
		watchProcesses( processes );
	}
} // namespace pdb100
