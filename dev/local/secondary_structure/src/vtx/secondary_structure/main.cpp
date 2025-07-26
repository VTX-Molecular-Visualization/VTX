#include <iostream>
#include <vtx/secondary_structure/files.hpp>
// #include <vtx/secondary_structure/parse.hpp>
#include <vtx/secondary_structure/report.hpp>
// #include <vtx/secondary_structure/tests.hpp>

int main()
{
	try
	{
		pdb100::Context context;
		pdb100::reporter( context.log ); // tag local log as global var
		std::cout << "Walk through pdb directory ... ";
		pdb100::enumerateFiles( context ); // makes a list of file to analyze
		std::cout << "done.\n";
		{ // Just for prompting
			auto fl = context.pdb100_system.open();
			std::cout << fl->size() << " pdb structures found.\n";
		}
		std::cout << "Starting threads ...\n";
		// pdb100::testSystems( context.pdb100_system ); // Start N threads as defined in the cmake
		std::cout << "Calculation is over.\n";
	}
	catch ( std::exception & e )
	{
		std::cout << "Error : " << e.what();
	}
	return 0;
}
