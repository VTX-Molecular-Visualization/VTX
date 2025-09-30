
#include <iostream>
#include <optional>
#include <util/chrono.hpp>
#include <vtx/secondary_structure/files.hpp>
#include <vtx/secondary_structure/process.hpp>
#include <vtx/secondary_structure/report.hpp>

int main()
{
	/* The program's workflow can be summarize like this :
	 * 1 - make a list of files to be analyzed
	 * 2 - make this list visible by other processes through shared memory
	 * 3 - create process that will mine the list and output report entries
	 * 4 - write a report file
	 */

	try
	{
		VTX::Util::Chrono chrono;
		chrono.start();
		pdb100::Context context;
		std::cout << "Walk through pdb directory ... ";
		pdb100::enumerateFiles( context ); // makes a list of file to analyze
		std::cout << "done.\n";
		{ // Just for prompting
			auto fl = context.pdb100_system.open();
			std::cout << fl->size() << " pdb structures found.\n";
		}
		std::cout << "Commencing calculation.\n";
		pdb100::testSystems( context );
		std::cout << "Calculation is over. It took " << chrono.elapsedTime() / 60000 << "min.\n";
	}
	catch ( std::exception & e )
	{
		std::cout << "Error : " << e.what();
	}
	std::cout << "Program exiting normally.\n";

	return 0;
}
