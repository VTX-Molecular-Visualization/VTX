#include <iostream>
#include <vtx/secondary_structure/files.hpp>
#include <vtx/secondary_structure/parse.hpp>
#include <vtx/secondary_structure/tests.hpp>

int main()
{
	try
	{
		pdb100::Context context;
		pdb100::reporter( context.log );
		pdb100::enumerateFiles( context );
		pdb100::testSystem( *context.pdb100_system.begin(), context.results );
	}
	catch ( std::exception & e )
	{
		std::cout << "Error : " << e.what();
	}
	return 0;
}
