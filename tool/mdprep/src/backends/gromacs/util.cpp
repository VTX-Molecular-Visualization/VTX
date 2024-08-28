#include <fstream>
#include <random>
#include <re2/re2.h>
//
#include "tool/mdprep/backends/gromacs/inputs.hpp"
//
#include "tool/mdprep/backends/gromacs/job.hpp"
#include "tool/mdprep/backends/gromacs/util.hpp"
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

namespace VTX::Tool::Mdprep::backends::Gromacs
{

	void replace( std::string & p_text, const char * p_pattern, const std::string & p_repl ) noexcept
	{
		size_t replPos = p_text.find( p_pattern );
		if ( replPos != std::string::npos )
			p_text.replace( replPos, strnlen_s( p_pattern, 0xff ), p_repl );
	}
	const fs::path g_defaultFfDirectoryRelativePath
		= ( fs::path( "data" ) / "tools" / "mdprep" / "gromacs" / "top" ).make_preferred();
	const fs::path & defaultFfDirectoryRelativePath() noexcept { return g_defaultFfDirectoryRelativePath; }

	const fs::path g_defaultGmxBinaryRelativePath
		= ( fs::path( "external" ) / "tools" / "mdprep" / "gromacs" / "gmx.exe" ).make_preferred();
	const fs::path & defaultGmxBinaryRelativePath() noexcept { return g_defaultGmxBinaryRelativePath; }

	const fs::path g_defaultGmxTemplatesRelativePath
		= fs::path { "data" } / "tools" / "mdprep" / "gromacs" / "templates";

	const fs::path & defaultGmxTemplatesRelativePath() noexcept { return g_defaultGmxTemplatesRelativePath; }

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
			const RE2 pattern { "(((Fatal +)|(\n( |\t)+))(E|e)rror(.|\n|\r)+?\r?\n)\r?\n" };

			absl::string_view txt_view( p_text );
			std::string		  hit;
			std::string		  b1, b2;
			while ( RE2::FindAndConsume( &txt_view, pattern, &hit ) )
			{
				while ( hit.starts_with( '\n' ) || hit.starts_with( '\r' ) || hit.starts_with( '\t' ) )
					hit.erase( hit.begin() );
				while ( hit.ends_with( '\n' ) || hit.ends_with( '\r' ) )
					hit.pop_back();
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
		return getFileOfType( p_list, 1, extension );
	}

	const std::string * getFileOfType(
		const CumulativeOuputFiles & p_list,
		const size_t &				 n,
		const char *				 suffix
	) noexcept
	{
		size_t suffixSeen = 0;
		for ( auto & it_fileStrPtr : p_list.fileStringPtrs )
		{
			if ( it_fileStrPtr->ends_with( suffix ) )
				suffixSeen++;
			if ( suffixSeen == n )
				return it_fileStrPtr;
		}
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
		while ( out.back() == '\0' )
			out.pop_back();

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

	fs::path createNewEmptyTempDirectory() noexcept
	{
		fs::path out = fs::temp_directory_path() / "VTX" / "tools" / "mdprep";
		fs::create_directories( out );
		std::random_device						rd;
		std::mt19937							gen( rd() );
		std::uniform_int_distribution<uint32_t> distrib( 0, 0xffffffff );
		std::string								uniqueDirName { std::to_string( distrib( gen ) ) };
		while ( fs::exists( out / uniqueDirName ) )
		{
			uniqueDirName = std::to_string( distrib( gen ) );
		}
		out /= uniqueDirName;
		fs::create_directories( out );
		return out.make_preferred();
	}
} // namespace VTX::Tool::Mdprep::backends::Gromacs
