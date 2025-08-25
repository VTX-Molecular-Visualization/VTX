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

		/**
		 * @brief Autmatically remove the shared object on program launch and on program teardown
		 */
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

		/**
		 * @brief Create a collection of files to be analyzed by child processes
		 * @param p_context
		 */
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
			fileStrDeque->reserve( contextData->size() );
			uint32_t it_idx = 0;
			for ( auto & it_filepathStr : *contextData )
			{
				if ( it_idx % STRUCTURE_STRIDE == 0 )
				{
					fileStrDeque->emplace_back( strAlloc ).assign( it_filepathStr.begin(), it_filepathStr.end() );
					// segfault ?
				}
				it_idx++;
			}
			p_context.initialFileCollectionSize = fileStrDeque->size();
			std::cout << "Created file collection of size <" << p_context.initialFileCollectionSize << ">.";
			if ( STRUCTURE_STRIDE > 1 )
				std::cout << "(stride downsampling was activated)";
			std::cout << "\n";
		}

		/**
		 * @brief Create a data structure in which child processes will be able to update their timestamp in order to
		 * show that they are still working on something.
		 * @param p_context
		 */
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

		/**
		 * @brief Create a map that shall contain the report entries. Although the map should be properly scaled to
		 * accomodate results, user should check the free space of a segment before appending a new entry
		 * @param p_context
		 */
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

			/**
			 * @brief ID of the process. No idea what it does on non-running one.
			 * @return
			 */
			ID getId() { return _proc.id(); }

			/**
			 * @brief Has the process terminated normally ?
			 * @return
			 */
			bool finished() { return _proc.exit_code() == 0; }

			/**
			 * @brief Is the process running ?
			 * @return
			 */
			bool running() { return _proc.running(); }

			/**
			 * @brief Method that stop the process synchronously.
			 */
			void kill()
			{
				// This method is kinda critical for the long run as it has been keen on crashing the parent

				// This implementation features the terminate method. I didn't see it crash the parent so far.
				std::cout << "Interrupting.\n";
				boost::process::error_code ec;
				_proc.terminate( ec );
				std::cout << "Interrupted : ec=" << ec << ".\n";
				std::cout << "Waiting interruption.\n";
				try
				{
					boost::process::error_code ec;
					_proc.wait( ec );
					std::cout << "Wait over : <" << ec << ">.\n ";
				}
				catch ( std::exception & e )
				{
					std::cout << "wait() crashed : " << e.what() << ".\n";
				}
				/*
				// This implementation doesn't make the parent crash but is not automatic anymore. The user will need to
				end each children when the parent is stuck. std::cout << "Interrupting.\n"; boost::process::error_code
				ec; _proc.request_exit( ec ); std::cout << "Interrupted : ec=" << ec << ".\n"; std::cout << "Waiting
				interruption.\n"; try
				{
					boost::process::error_code ec;
					_proc.wait( ec );
				}
				catch ( std::exception & e )
				{
					std::cout << "wait() crashed : " << e.what() << ".\n";
				}
				std::cout << "Wait over.\n";
				*/
			}

			/**
			 * @brief Start a new process. Assumes the previous one is not running.
			 */
			void restart()
			{
				std::cout << "Instanciating new process.\n";
				_proc = boost::process::process( _ctxt->get_executor(), _path, {} );
				std::cout << "New process instanciated.\n";
			}

		  private:
			boost::asio::io_context * _ctxt;
			boost::process::process	  _proc;
			int						  _lastExitCode = 1;
		};

		/**
		 * @brief Function that tries to lock on a mutex but reset it after a certain number of failures.
		 * @param ml
		 * @param p_name
		 */
		void maybeResetMutex( MutexAndLock & ml, const char * p_name )
		{
			ml.mutex.emplace( open_or_create, p_name );
			ml.lock.emplace( ml.mutex.value(), try_to_lock );
			const uint8_t  WAITING_TIME_S = 10, SLEEPING_TIME_MS = 10;
			const uint32_t MAX_ATTEMPTS = 1000 * WAITING_TIME_S / SLEEPING_TIME_MS;
			uint32_t	   numAttempts	= 0;
			while ( not ml.lock.value() )
			{
				if ( numAttempts % 100 == 0 )
					std::cout << "Watching Mutex <" << p_name << ">.\n";
				std::this_thread::sleep_for( std::chrono::milliseconds( SLEEPING_TIME_MS ) );
				ml.lock.emplace( ml.mutex.value(), try_to_lock );

				numAttempts++;
				if ( numAttempts > MAX_ATTEMPTS )
					break;
			}
			if ( not ml.lock.value() )
			{
				try
				{
					boost::interprocess::named_mutex::remove( p_name );
					ml.mutex.emplace( create_only, p_name );
					ml.lock.emplace( ml.mutex.value() );
					std::cout << "Mutex <" << p_name << "> was reset.\n";
				}
				catch ( std::exception & e )
				{
					std::cout << "Mutex <" << p_name << "> removal failed : " << e.what() << "\n.";
				}
			}
		}

		/**
		 * @brief Look for processes that didn't give proof of life since at least shm::livingProof::tolerenceTime
		 * seconds, kill them and restart them.
		 * @param p_processes
		 */
		void restartCrashedProcess( std::array<RestartingProcess, NUM_PROCESSES> & p_processes )
		{
			MutexAndLock ml;
			maybeResetMutex( ml, shm::livingProof::MUTEX );

			std::stack<uint64_t> removeKeys; // Used to cleanup the Proof of life map

			boost::interprocess::managed_shared_memory sharedSegment(
				boost::interprocess::open_only, pdb100::shm::livingProof::SEGNAME
			);
			auto livingProofMapPair = sharedSegment.find<LivingProofMap>( pdb100::shm::livingProof::OBJNAME );

			for ( auto & it_pair : *livingProofMapPair.first )
			{
				uint64_t timeSinceLastProofOfLife = getTimeStamp() - it_pair.second;
				if ( timeSinceLastProofOfLife < shm::livingProof::tolerenceTime )
					continue;

				removeKeys.push( it_pair.first );
				auto findRslt = std::find_if(
					p_processes.begin(),
					p_processes.end(),
					[ id = it_pair.first ]( RestartingProcess & p ) { return p.getId() == id; }
				);
				if ( findRslt != std::end( p_processes ) )
				{
					RestartingProcess & child = *findRslt;
					std::cout << "Restarting process <" << it_pair.first << "> that didn't give proof of life since "
							  << timeSinceLastProofOfLife << "s\n";
					if ( child.running() )
					{
						MutexAndLock ml[ 2 ];
						maybeResetMutex( ml[ 0 ], shm::rsltMap::MUTEX );
						maybeResetMutex( ml[ 1 ], shm::filestrDeque::MUTEX );
						try
						{
							child.kill(); // Best part
						}
						catch ( std::exception & e )
						{
							std::cout << "Kill failed : " << e.what() << "\n";
						}
					}
					child.restart();
				}
				else
				{
					std::cout << "Process attached to pid <" << it_pair.first << "> not found. Weird.\n";
				}
			}
			// Remove process(es) that were killed so the map doesn't grow
			while ( not removeKeys.empty() )
			{
				livingProofMapPair.first->erase( removeKeys.top() );
				removeKeys.pop();
			}
		}
		/**
		 * @brief Returns the number of structure to be used left. Does lock (and maybe reset) the file queue mutex
		 * @return Number of struct left
		 */
		size_t getNumberOfStructureLeftInQueue()
		{
			MutexAndLock ml;
			maybeResetMutex( ml, shm::filestrDeque::MUTEX );
			boost::interprocess::managed_shared_memory sharedSegment(
				boost::interprocess::open_only, pdb100::shm::filestrDeque::SEGNAME
			);
			auto fileStrDeque = sharedSegment.find<StringDeque>( pdb100::shm::filestrDeque::OBJNAME );
			return fileStrDeque.first->size();
		}
		/**
		 * @brief Print computation progress on the stdout
		 * @param p_initialStructureNumber Number of
		 */
		void showProgress( const size_t & p_initialStructureNumber )
		{
			size_t currentSize = getNumberOfStructureLeftInQueue();
			std::cout << fmt::format(
				"Progression : {:0.2f}% ({}/{})\n",
				100. * ( p_initialStructureNumber - currentSize ) / p_initialStructureNumber,
				( p_initialStructureNumber - currentSize ),
				p_initialStructureNumber
			);
		}

		/**
		 * @brief Since processes can have a mutex locked while crashing or being interrupted, we need to check if each
		 * mutex can be locked in a reasonnable time, and reset them if not.
		 */
		void checkMutexes()
		{
			MutexAndLock ml[ 3 ];
			maybeResetMutex( ml[ 0 ], shm::livingProof::MUTEX );
			maybeResetMutex( ml[ 1 ], shm::rsltMap::MUTEX );
			maybeResetMutex( ml[ 2 ], shm::filestrDeque::MUTEX );
		}

		/**
		 * @brief Look at processes timestamp to detect processes that crashed and restart it if needed. Yield once all
		 * processes are finished
		 * @tparam SIZE
		 * @param p_processes
		 */
		void watchProcesses( const Context & p_context, std ::array<RestartingProcess, NUM_PROCESSES> & p_processes )
		{
			auto lastProgressPrint = getTimeStamp();
			while ( true )
			{
				std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
				restartCrashedProcess( p_processes );
				bool finished = true;
				if ( getNumberOfStructureLeftInQueue() == 0 )
				{
					bool running = false;
					for ( auto & it_proc : p_processes )
					{
						running |= it_proc.running();
					}
					if ( not running )
						break;
				}
				if ( lastProgressPrint + showProgressInterval < getTimeStamp() )
				{
					lastProgressPrint = getTimeStamp();
					showProgress( p_context.initialFileCollectionSize );
				}
			}
			std::cout << "Exiting process watching.\n";
		}

		/**
		 * @brief Here to forward initialization of a process.
		 * @tparam T The T argument is only here to statically unpack the call of the function the required number of
		 * times
		 * @param p_arg
		 * @return An instanciated process
		 */
		template<typename T>
		RestartingProcess _generate( boost::asio::io_context & p_arg, T )
		{
			return { p_arg };
		}
		/**
		 * @brief Call statically the ctor of a process NUM_PROCESSES times
		 * @tparam ...array_pack Collection of int with static size
		 * @param p_arg Used to instantiate a process
		 */
		template<std::size_t... array_pack>
		std::array<RestartingProcess, NUM_PROCESSES> _generate(
			boost::asio::io_context & p_arg,
			std::index_sequence<array_pack...>
		)
		{
			return { _generate( p_arg, array_pack )... };
		}

		/**
		 * @brief Initialize the array of processes and return it.
		 * @param p_arg Used to instantiate a process
		 */
		std::array<RestartingProcess, NUM_PROCESSES> generate( boost::asio::io_context & p_arg )
		{
			return _generate( p_arg, std::make_index_sequence<NUM_PROCESSES> {} );
		}

	} // namespace

	void testSystems( Context & p_context ) noexcept
	{
		// We create Shared memory objects for the multiprocessing part.
		shm::createFileStrCollection( p_context );
		shm::createLivingProofCollection( p_context );
		shm::createResultMap( p_context );

		{
		startProcesses:
			boost::asio::io_context io_context; // Required to start processes
			auto					processes = generate( io_context );
			std::cout << "Starting process watching.\n";
			watchProcesses( p_context, processes );
		}
		std::cout << "Process watching over.\n";
		if ( getNumberOfStructureLeftInQueue() > 0 )
		{
			std::cout << "Some structures are left. Restarting process watching.\n";
			goto startProcesses;
		}
	}
} // namespace pdb100
