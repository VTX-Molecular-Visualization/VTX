#include <fstream>
#include <re2/re2.h>
//
#include "tools/mdprep/gromacs/inputs.hpp"
//
#include "tools/mdprep/gromacs/job.hpp"
#include "tools/mdprep/gromacs/util.hpp"
#include <qapplication.h>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>

namespace VTX::Tool::Mdprep
{
	std::optional<fs::path> g_executableDirectory;
	const fs::path &		executableDirectory() noexcept
	{
		if ( !g_executableDirectory.has_value() )
		{
			g_executableDirectory = VTX::Util::Filesystem::getExecutableDir();
			g_executableDirectory->make_preferred();
		}
		return g_executableDirectory.value();
	}

} // namespace VTX::Tool::Mdprep

namespace VTX::Tool::Mdprep::Gromacs
{

	const fs::path g_defaultFfDirectoryRelativePath
		= ( fs::path( "data" ) / "tools" / "mdprep" / "gromacs" / "top" ).make_preferred();
	const fs::path & defaultFfDirectoryRelativePath() noexcept { return g_defaultFfDirectoryRelativePath; }

	const fs::path g_defaultGmxBinaryRelativePath
		= ( fs::path( "external" ) / "tools" / "mdprep" / "gromacs" / "gmx.exe" ).make_preferred();
	const fs::path & defaultGmxBinaryRelativePath() noexcept { return g_defaultGmxBinaryRelativePath; }

	void declareFfDirectory( const std::filesystem::path & p_path ) noexcept
	{
		std::string	   pathStr = p_path.string();
		QByteArrayView env_arg( pathStr.data(), pathStr.data() + pathStr.size() );
		qputenv( "GMXLIB", env_arg );
	}

	namespace
	{
		void checkErrMsg( JobReport & p_report, const std::string & p_text ) noexcept
		{
			const RE2 pattern { "\n(( |\t)+Error (.|\n|\r)+?\n\n)" };

			absl::string_view txt_view( p_text );
			std::string		  hit;
			std::string		  b1, b2;
			while ( RE2::FindAndConsume( &txt_view, pattern, &hit ) )
			{
				p_report.errors.push_back( hit );
			}
		}
	} // namespace

	void checkJobResults( GromacsJobData & p_in ) noexcept
	{
		{
			auto channels = p_in.channelsLocker.open();
			checkErrMsg( p_in.report, channels->stderr_ );
			checkErrMsg( p_in.report, channels->stdout_ );
		}

		for ( auto & it : p_in.expectedOutputFilesIndexes )
		{
			fs::path f { p_in.arguments[ it ] };
			if ( f.empty() )
				continue;
			if ( fs::exists( f ) == false || fs::is_regular_file( f ) == false )
			{
				p_in.report.errors.push_back(
					fmt::format( "Expected output file <{}> not found.", p_in.arguments[ it ] )
				);
				break;
			}
			if ( fs::file_size( f ) == 0 )
			{
				p_in.report.errors.push_back( fmt::format( "Expected output file <{}> is empty.", p_in.arguments[ it ] )
				);
				break;
			}
		}
		p_in.report.errorOccured = ( p_in.report.errors.empty() == false );
	}

	const std::string * getFirstFileOfType( const CumulativeOuputFiles & p_list, const char * extension ) noexcept
	{
		auto firstSuffixedString = std::find_if(
			p_list.fileStringPtrs.begin(),
			p_list.fileStringPtrs.end(),
			[ extension = extension ]( const std::string * p_ ) { return p_->ends_with( extension ); }
		);
		if ( firstSuffixedString != std::end( p_list.fileStringPtrs ) )
			return *firstSuffixedString;
		return nullptr;
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

	void writeIntoFile( const fs::path & p_file, const std::string & p_content ) noexcept
	{
		std::ofstream( p_file ) << p_content;
	}

	void setLastArgumentAsExpectedOutputFile( GromacsJobData & p_ ) noexcept
	{
		p_.expectedOutputFilesIndexes.push_back( p_.arguments.size() - 1 );
	}

} // namespace VTX::Tool::Mdprep::Gromacs
