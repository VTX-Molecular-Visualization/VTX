
#include "mdprep/test/fixture.hpp"
#include <tools/mdprep/gromacs/util.hpp>

namespace VTX::test
{
	PrepareJobSetup::PrepareJobSetup( const char * p_rootDirName, const char * jobName ) :
		jobName( jobName ), rootDir( VTX::Tool::Mdprep::executableDirectory() / "out" / "prepareJob" / p_rootDirName ),
		jobDir( rootDir / jobName )
	{
		if ( fs::exists( jobDir ) )
			fs::remove_all( jobDir );
	}
	void fill( const std::vector<std::string> & p_in, std::vector<const std::string *> & p_out ) noexcept
	{
		p_out.reserve( p_out.size() + p_in.size() );
		for ( auto & it : p_in )
			p_out.push_back( &it );
	}

} // namespace VTX::test
