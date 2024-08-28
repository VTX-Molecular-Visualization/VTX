
#include "mdprep/test/fixture.hpp"
#include <fstream>
#include <tools/mdprep/backends/gromacs/util.hpp>
namespace VTX::test
{
	PrepareJobSetup::PrepareJobSetup( const char * p_rootDirName, const char * jobName ) :
		jobName( jobName ), rootDir( VTX::Tool::Mdprep::executableDirectory() / "out" / "prepareJob" / p_rootDirName ),
		jobDir( rootDir / jobName )
	{
		if ( fs::exists( jobDir ) )
			fs::remove_all( jobDir );
	}
	void fill(
		const std::vector<std::string> &				   p_in,
		VTX::Tool::Mdprep::backends::Gromacs::CumulativeOuputFiles & p_out
	) noexcept
	{
		p_out.fileStringPtrs.reserve( p_out.fileStringPtrs.size() + p_in.size() );
		for ( auto & it : p_in )
		{
			if ( p_out.lastUncompiledTop.empty() && it.ends_with( ".top" ) == true )
				p_out.lastUncompiledTop = it;
			p_out.fileStringPtrs.push_back( &it );
		}
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
