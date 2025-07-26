#include <array>
#include <boost/process/process.hpp>
#include <thread>
#include <vtx/secondary_structure/shared.hpp>
#include <vtx/secondary_structure/shared/interprocess.hpp>
//
#include <vtx/secondary_structure/process.hpp>

namespace pdb100
{
	namespace shm
	{
		void createFileStrCollection( Context & p_context ) {}
		void createLivingProofCollection( Context & p_context ) {}
		void createResultMap( Context & p_context ) {}
	} // namespace shm
	namespace
	{
		class RestartingProcess
		{
		  public:
			RestartingProcess() {}

			using ID = boost::interprocess::ipcdetail::OS_process_id_t;

			ID	 getId() { return 1; }
			bool finished() { return true; }

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

	} // namespace

	void testSystems( Context & p_context ) noexcept
	{
		shm::createFileStrCollection( p_context );
		shm::createLivingProofCollection( p_context );
		shm::createResultMap( p_context );

		std::array<RestartingProcess, NUM_PROCESSES> processes;
		watchProcesses( processes );
	}
} // namespace pdb100
