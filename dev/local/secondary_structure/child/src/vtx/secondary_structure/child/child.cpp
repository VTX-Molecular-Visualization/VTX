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
	bool g_testsOver = false;
	void startLivingProofPosting( std::jthread & p_thr )
	{
		p_thr = std::jthread(
			[]
			{
				uint64_t processId = boost::process::current_pid();
				while ( not g_testsOver )
				{
					boost::interprocess::named_mutex mutex( open_or_create, shm::livingProof::MUTEX );
					boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock( mutex );
					boost::interprocess::managed_shared_memory						   sharedSegment(
						boost::interprocess::open_only, pdb100::shm::livingProof::SEGNAME
					);
					auto livingProofMapPair = sharedSegment.find<LivingProofMap>( pdb100::shm::livingProof::OBJNAME );

					livingProofMapPair.first->at( processId ) = getTimeStamp();
					std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
				}
			}
		);
	}
	void createLivingProofEntry()
	{
		uint64_t						 processId = boost::process::current_pid();
		boost::interprocess::named_mutex mutex( open_or_create, shm::livingProof::MUTEX );
		boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock( mutex );
		boost::interprocess::managed_shared_memory						   sharedSegment(
			boost::interprocess::open_only, pdb100::shm::livingProof::SEGNAME
		);
		auto livingProofMapPair = sharedSegment.find<LivingProofMap>( pdb100::shm::livingProof::OBJNAME );

		if ( not livingProofMapPair.first->contains( processId ) )
			livingProofMapPair.first->emplace( processId, getTimeStamp() );
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
	try
	{
		pdb100::log() << "Starting process\n";
		pdb100::createLivingProofEntry();
		std::jthread thread;
		pdb100::startLivingProofPosting( thread );

		pdb100::testSystems();
	}
	catch ( std::exception & e )
	{
		pdb100::log() << "Exception catched : <" << e.what() << ">\n";
	}
	pdb100::g_testsOver = true;
	return 0;
}
