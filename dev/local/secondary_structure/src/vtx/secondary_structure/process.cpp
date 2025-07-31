//
#include <boost/asio.hpp>
#include <boost/process/v2/process.hpp>
//
#include <array>
//
#include <boost/interprocess/sync/named_mutex.hpp>
#include <vtx/secondary_structure/shared.hpp>
#include <vtx/secondary_structure/shared/interprocess.hpp>
//
#include <iostream>
#include <thread>
#include <vtx/secondary_structure/process.hpp>

namespace pdb100
{
	namespace shm
	{

		struct shm_remove
		{
			shm_remove() { remove_all_shit(); }
			~shm_remove() { remove_all_shit(); }
			void remove_all_shit()
			{
				boost::interprocess::shared_memory_object::remove( pdb100::shm::filestrDeque::SEGNAME );
				boost::interprocess::named_mutex::remove( pdb100::shm::filestrDeque::MUTEX );
				boost::interprocess::shared_memory_object::remove( pdb100::shm::livingProof::SEGNAME );
				boost::interprocess::named_mutex::remove( pdb100::shm::livingProof::MUTEX );
				boost::interprocess::shared_memory_object::remove( pdb100::shm::rsltMap::SEGNAME );
				boost::interprocess::named_mutex::remove( pdb100::shm::rsltMap::MUTEX );
			}
		} g_shared_mem_remover;
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
			const size_t shm_size
				= ( NUM_PROCESSES * 2 ) * ( sizeof( std::pair<uint64_t, uint64_t> ) ) + sizeof( LivingProofMap ) + 1000;

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
			const size_t shm_size
				= contextData->size() * ( contextData->back().size() + sizeof( ReportItem<String> ) + 50 )
				  + sizeof( ReportItemCollection ) + 1000;
			boost::interprocess::managed_shared_memory sharedSegment(
				boost::interprocess::create_only, pdb100::shm::rsltMap::SEGNAME, shm_size
			);
			ReportItemAllocator	   mapAllocator( sharedSegment.get_segment_manager() );
			ReportItemCollection * map
				= sharedSegment.construct<ReportItemCollection>( pdb100::shm::rsltMap::OBJNAME )( mapAllocator );
			map->reserve( contextData->size() );
		}
	} // namespace shm
	namespace
	{
		class RestartingProcess
		{
			static inline const std::string _path { CHILD_PROCESS_NAME ".exe" };

		  public:
			RestartingProcess( boost::asio::io_context & p_ctx ) :
				_ctxt( &p_ctx ), _proc( p_ctx.get_executor(), _path, {} )
			{
			}

			using ID = boost::interprocess::ipcdetail::OS_process_id_t;

			ID	 getId() { return _proc.id(); }
			bool finished() { return _proc.exit_code() == 0; }
			bool running() { return _proc.running(); }
			void kill()
			{
				_proc.terminate();
				_proc.wait();
				//_lastExitCode = _proc.exit_code();
			}
			void restart() { _proc = boost::process::process( _ctxt->get_executor(), _path, {} ); }

		  private:
			boost::asio::io_context * _ctxt;
			boost::process::process	  _proc;
			// int						  _lastExitCode = 1;
		};

		void restartCrashedProcess( std::array<RestartingProcess, NUM_PROCESSES> & p_processes )
		{
			boost::interprocess::named_mutex mutex( open_or_create, shm::livingProof::MUTEX );
			boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock( mutex );
			boost::interprocess::managed_shared_memory						   sharedSegment(
				boost::interprocess::open_only, pdb100::shm::livingProof::SEGNAME
			);
			auto livingProofMapPair		   = sharedSegment.find<LivingProofMap>( pdb100::shm::livingProof::OBJNAME );
			constexpr uint64_t	 NO_REMOVE = 0xffffffffffffffff;
			std::stack<uint64_t> removeKeys;
			for ( auto & it_pair : *livingProofMapPair.first )
			{
				uint64_t timestamp = getTimeStamp();
				if ( timestamp > it_pair.second + shm::livingProof::tolerenceTime )
				{
					removeKeys.push( it_pair.first );
					auto findRslt = std::find_if(
						p_processes.begin(),
						p_processes.end(),
						[ id = it_pair.first ]( RestartingProcess & p ) { return p.getId() == id; }
					);
					if ( findRslt != std::end( p_processes ) )
					{
						RestartingProcess & child = *findRslt;
						std::cout << "Restarting process <" << it_pair.first
								  << "> that didn't give proof of life since " << timestamp - it_pair.second << "s\n";
						if ( child.running() )
						{
							child.kill(); // Best part
						}
						child.restart();
					}
					else
					{
						std::cout << "Process attached to pid <" << it_pair.first << "> not found. Weird.\n";
					}
				}
			}
			while ( not removeKeys.empty() )
			{
				livingProofMapPair.first->erase( removeKeys.top() );
				removeKeys.pop();
			}
		}

		/**
		 * @brief Look at processes timestamp to detect processes that crashed and restart it if needed. Yield once all
		 * processes are finished
		 * @tparam SIZE
		 * @param p_processes
		 */
		void watchProcesses( std ::array<RestartingProcess, NUM_PROCESSES> & p_processes )
		{
			while ( true )
			{
				std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
				restartCrashedProcess( p_processes );
				bool finished = true;
				for ( auto & it_process : p_processes )
				{
					finished &= ( not it_process.running() ) and it_process.finished();
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
