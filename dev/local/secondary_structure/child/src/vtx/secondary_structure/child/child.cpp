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
	const fs::path g_logPath( "ChildLog.log" );
	std::jthread   g_livingProofThread;
	bool		   g_testsOver = false;
	std::ofstream  log() { return std::ofstream( g_logPath, std::ios::app ); }
	void		   startLivingProofPosting()
	{
		log() << "Starting living thread\n";
		g_livingProofThread = std::jthread(
			[]
			{
				uint64_t processId = boost::process::current_pid();
				while ( not g_testsOver )
				{
					boost::interprocess::named_mutex		   mutex( open_or_create, shm::livingProof::MUTEX );
					boost::interprocess::managed_shared_memory sharedSegment(
						boost::interprocess::open_only, pdb100::shm::livingProof::SEGNAME
					);
					auto livingProofMapPair = sharedSegment.find<LivingProofMap>( pdb100::shm::livingProof::OBJNAME );

					if ( not livingProofMapPair.first->contains( processId ) )
						livingProofMapPair.first->emplace( processId, getTimeStamp() );
					else
						livingProofMapPair.first->at( processId ) = getTimeStamp();
					log() << "LivingProof sent.\n";
					std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
				}
			}
		);
	}
} // namespace pdb100

int main()
{
	/*
	WIP
	Shall start another thread that update shared timestamp every 3 seconds (arbitrary)

	Fetch a system
		Then, add a result into the shared result list with a crash status
		Then test the system
		Then, update the result map with specific statistics
		Rinse and repeat

	*/
	pdb100::startLivingProofPosting();
	pdb100::log() << "Living thread started\n";

	pdb100::testSystems();
	pdb100::g_testsOver = true;
	return 0;
}
