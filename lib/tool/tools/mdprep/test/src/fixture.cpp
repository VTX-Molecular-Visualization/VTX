
#include "mdprep/test/fixture.hpp"
#include <fstream>
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
	std::string getFileContent( const fs::path & p_file ) noexcept
	{
		if ( fs::exists( p_file ) == false )
			return {};
		std::ifstream strm { p_file, std::ios::ate };
		size_t		  fileSize = strm.tellg();
		std::string	  out( fileSize, '\0' );
		strm.seekg( 0 );
		strm.read( out.data(), fileSize );
		return out;
	}

} // namespace VTX::test
