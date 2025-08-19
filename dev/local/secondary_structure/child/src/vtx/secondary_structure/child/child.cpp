#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <boost/process/v2/pid.hpp>
//
#include <boost/interprocess/sync/named_mutex.hpp>
#include <fstream>
#include <thread>
#include <vtx/secondary_structure/child/tests.hpp>
#include <vtx/secondary_structure/shared/interprocess.hpp>

namespace pdb100
{
	/**
	 * @brief Start a thread that will be responsible for updating the livingproof entry as long as it can, and shall
	 * stop when the computation is over.
	 * @param p_thr
	 */
	void startLivingProofPosting( std::jthread & p_thr )
	{
		p_thr = std::jthread(
			[]( std::stop_token token )
			{
				uint64_t processId = boost::process::current_pid();
				while ( not token.stop_requested() )
				{
					std::this_thread::sleep_for( std::chrono::seconds( shm::livingProof::tolerenceTime / 2 ) );
					{
						LazyLock<shm::livingProof::MUTEX>		   lock;
						boost::interprocess::managed_shared_memory sharedSegment(
							boost::interprocess::open_only, pdb100::shm::livingProof::SEGNAME
						);
						auto livingProofMapPair
							= sharedSegment.find<LivingProofMap>( pdb100::shm::livingProof::OBJNAME );

						livingProofMapPair.first->at( processId ) = getTimeStamp();
					}
				}
			}
		);
	}

	/**
	 * @brief Lock the livingProof mutex and create an entry
	 */
	void createLivingProofEntry()
	{
		uint64_t								   processId = boost::process::current_pid();
		LazyLock<shm::livingProof::MUTEX>		   lock;
		boost::interprocess::managed_shared_memory sharedSegment(
			boost::interprocess::open_only, pdb100::shm::livingProof::SEGNAME
		);
		auto livingProofMapPair = sharedSegment.find<LivingProofMap>( pdb100::shm::livingProof::OBJNAME );

		if ( not livingProofMapPair.first->contains( processId ) )
			livingProofMapPair.first->emplace( processId, getTimeStamp() );
	}
} // namespace pdb100

#include <iostream>
int main()
{
	/* Process meant to be spawned by the secondary structure parent. It goes as follow :
	 * 1 - Create an entry in the living proof data structure
	 * 2 - Spawn a thread that will improve this entry
	 * 3 - Mine a file from the file shared memory collection
	 * 4 - Analyze it with VTX algorithm and by reading PDB information
	 * 5 - Compare VTX result with information from PDB
	 * 6 - Add report entry into the shared memory report entry list
	 * 7 - Restart from 3 until there is no file left
	 *
	 * This process is meant to crash at some point. The idea is for the parent to see that the process is crashed to
	 * restart it.
	 */
	try
	{
		std::jthread thread;
		pdb100::log() << "Starting process\n";
		pdb100::createLivingProofEntry();
		pdb100::startLivingProofPosting( thread );

		pdb100::testSystems();
	}
	catch ( std::exception & e )
	{
		pdb100::log() << "Exception catched : <" << e.what() << ">\n";
		return 1;
	}
	return 0;
}
