#include <iostream>
#include <vtx/secondary_structure/files.hpp>
#include <vtx/secondary_structure/parse.hpp>
#include <vtx/secondary_structure/tests.hpp>

int main()
{
	try
	{
		pdb100::Context context;
		pdb100::reporter( context.log ); // Makes it global
		pdb100::enumerateFiles( context );
		for ( auto & s : context.pdb100_system )
			pdb100::testSystem( s, context.results );
	}
	catch ( std::exception & e )
	{
		std::cout << "Error : " << e.what();
	}
	return 0;
}
