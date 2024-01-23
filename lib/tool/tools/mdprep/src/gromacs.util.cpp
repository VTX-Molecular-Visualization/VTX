#include "tools/mdprep/gromacs.util.hpp"
#include "tools/mdprep/gromacs.hpp"
#include <algorithm>
#include <filesystem>
#include <format>
#include <optional>
#include <qapplication.h>
#include <regex>
#include <set>
#include <util/exceptions.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>

namespace VTX::Tool::Mdprep
{
	std::optional<fs::path> g_executableDirectory;
	const fs::path &		executableDirectory() noexcept
	{
		if ( !g_executableDirectory.has_value() )
		{
			// int				 argc = 0;
			// QCoreApplication app( argc, nullptr );
			// QString			 qpath = app.applicationDirPath();
			QString		qpath = QCoreApplication::applicationDirPath();
			auto		tmp	  = qpath.toLocal8Bit();
			std::string path_str( tmp.data(), tmp.size() );
			g_executableDirectory.emplace( path_str );
			g_executableDirectory->make_preferred();
		}
		return g_executableDirectory.value();
	}

} // namespace VTX::Tool::Mdprep

namespace VTX::Tool::Mdprep::Gromacs
{
	namespace
	{
		const char g_ff_suffix[] = ".ff";
	}

	const fs::path g_defaultFfDirectoryRelativePath
		= ( fs::path( "data" ) / "tools" / "mdprep" / "gromacs" / "top" ).make_preferred();
	const fs::path & defaultFfDirectoryRelativePath() noexcept { return g_defaultFfDirectoryRelativePath; }

	const fs::path g_defaultGmxBinaryRelativePath
		= ( fs::path( "external" ) / "tools" / "mdprep" / "gromacs" / "gmx.exe" ).make_preferred();
	const fs::path & defaultGmxBinaryRelativePath() noexcept { return g_defaultGmxBinaryRelativePath; }

	void declareFfDirectory( const std::filesystem::path & p_path ) noexcept
	{
		std::string	   path_str = p_path.string();
		QByteArrayView env_arg( path_str.data(), path_str.data() + path_str.size() );
		qputenv( "GMXLIB", env_arg );
	}

	std::vector<forcefield> listForcefields( const fs::path & p_data_dir )
	{
		if ( !fs::is_directory( p_data_dir ) )
		{
			auto err_str = std::format( "Directory <{}> not found", p_data_dir.string() );
			throw VTX::IOException( err_str );
		}
		std::vector<forcefield> out;

		for ( auto & fs_element : fs::directory_iterator( p_data_dir ) )
		{
			if ( !fs_element.is_directory() )
				continue;

			auto filename = fs_element.path().filename().string();
			if ( filename.ends_with( g_ff_suffix ) )
			{
				out.emplace_back( forcefield { .forcefieldFolderPath = fs_element.path().string() } );
				continue;
			}
		}

		return out;
	}

	const char * string( const E_WATER_MODEL & p_w ) noexcept
	{
		switch ( p_w )
		{
		case VTX::Tool::Mdprep::Gromacs::E_WATER_MODEL::none: return "none";
		case VTX::Tool::Mdprep::Gromacs::E_WATER_MODEL::spc: return "spc";
		case VTX::Tool::Mdprep::Gromacs::E_WATER_MODEL::spce: return "spce";
		case VTX::Tool::Mdprep::Gromacs::E_WATER_MODEL::tip3p: return "tip3p";
		case VTX::Tool::Mdprep::Gromacs::E_WATER_MODEL::tip4p: return "tip4p";
		case VTX::Tool::Mdprep::Gromacs::E_WATER_MODEL::tip5p: return "tip5p";
		case VTX::Tool::Mdprep::Gromacs::E_WATER_MODEL::tips3p: return "tips3p";
		default: break;
		}
		return "";
	}

	void convert( const Pdb2gmxInstructions & p_in, GromacsCommandArgs & p_out ) noexcept
	{
		if ( p_in.forcefields.empty() )
			return;
		if ( p_in.forcefieldIndex >= p_in.forcefields.size() )
			return;
		if ( p_in.inputPdb.empty() )
			return;
		if ( !p_in.inputPdb.has_filename() )
			return;

		fs::path outputDir = p_in.outputDir;
		if ( outputDir.empty() )
			outputDir = p_in.inputPdb.parent_path();
		fs::create_directories( outputDir );
		p_out.arguments.clear();

		p_out.arguments.push_back( "pdb2gmx" );
		p_out.arguments.push_back( "-f" );
		p_out.arguments.push_back( p_in.inputPdb.string() );
		std::string input_root_name = p_in.inputPdb.filename().string();
		p_out.arguments.push_back( "-o" );
		p_out.arguments.push_back( ( outputDir / ( input_root_name + ".gro" ) ).string() );
		p_out.arguments.push_back( "-p" );
		p_out.arguments.push_back( ( outputDir / ( input_root_name + ".top" ) ).string() );
		p_out.arguments.push_back( "-i" );
		p_out.arguments.push_back( ( outputDir / ( input_root_name + ".itp" ) ).string() );
		p_out.arguments.push_back( "-q" );
		p_out.arguments.push_back( ( outputDir / ( input_root_name + ".clean.pdb" ) ).string() );
		p_out.arguments.push_back( "-n" );
		p_out.arguments.push_back( ( outputDir / ( input_root_name + ".ndx" ) ).string() );
		p_out.arguments.push_back( "-ff" );
		p_out.arguments.push_back( p_in.forcefields.at( p_in.forcefieldIndex ).getName().data() );
		p_out.arguments.push_back( "-water" );
		p_out.arguments.push_back( string( p_in.water ) );

		if ( p_in.customParameter.has_value() )
		{
			std::set<E_INTERACTIVE_KEYWORD> seen_kw;
			p_out.interactiveSettings = p_in.customParameter;
			for ( auto & it : p_in.customParameter->kwValue )
			{
				if ( seen_kw.contains( it.first.kw ) )
					continue;

				p_out.arguments.push_back( std::string( "-" ) += string( it.first.kw ) );
			}
		}
	}
	void convert( const solvateInstructions &, GromacsCommandArgs & ) noexcept {}

	const char * string( const E_INTERACTIVE_KEYWORD & p_kw ) noexcept
	{
		switch ( p_kw )
		{
		case E_INTERACTIVE_KEYWORD::ss: return "ss";
		case E_INTERACTIVE_KEYWORD::ter: return "ter";
		case E_INTERACTIVE_KEYWORD::lys: return "lys";
		case E_INTERACTIVE_KEYWORD::arg: return "arg";
		case E_INTERACTIVE_KEYWORD::asp: return "asp";
		case E_INTERACTIVE_KEYWORD::glu: return "glu";
		case E_INTERACTIVE_KEYWORD::gln: return "gln";
		case E_INTERACTIVE_KEYWORD::his: return "his";
		default: return "";
		}
		return "";
	}

	std::string_view forcefield::getName() const
	{
		size_t filename_pos
			= this->forcefieldFolderPath.find( fs::path( this->forcefieldFolderPath ).filename().string() );
		size_t extension_pos = this->forcefieldFolderPath.size() - ( sizeof( g_ff_suffix ) - 1 );
		return std::string_view { std::next( this->forcefieldFolderPath.begin(), filename_pos ),
								  std::next( this->forcefieldFolderPath.begin(), extension_pos ) };
	}

	void parse( const std::string & p_user_str, E_INTERACTIVE_KEYWORD & p_out ) noexcept
	{
		if ( p_user_str == "HIS" )
		{
			p_out = E_INTERACTIVE_KEYWORD::his;
			return;
		};
		if ( p_user_str == "LYS" )
		{
			p_out = E_INTERACTIVE_KEYWORD::lys;
			return;
		};
		if ( p_user_str == "ASP" )
		{
			p_out = E_INTERACTIVE_KEYWORD::asp;
			return;
		};
		if ( p_user_str == "GLN" )
		{
			p_out = E_INTERACTIVE_KEYWORD::gln;
			return;
		};
		if ( p_user_str == "GLU" )
		{
			p_out = E_INTERACTIVE_KEYWORD::glu;
			return;
		};
		if ( p_user_str == "ARG" )
		{
			p_out = E_INTERACTIVE_KEYWORD::arg;
			return;
		};
		if ( p_user_str == "TER" )
		{
			p_out = E_INTERACTIVE_KEYWORD::ter;
			return;
		};
		if ( p_user_str == "SS" )
		{
			p_out = E_INTERACTIVE_KEYWORD::ss;
			return;
		};
		p_out = E_INTERACTIVE_KEYWORD::none;
		return;
	}

	parseReport parsePdb2gmxUserScript( const std::string_view & p_script, InteractiveArguments & p_args ) noexcept
	{
		parseReport out;

		p_args.kwValue.clear();
		if ( p_script.empty() )
			return out;

		std::string_view::const_iterator current_pos	  = p_script.begin(),
										 next_newline_pos = std::find( p_script.begin(), p_script.end(), '\n' );
		if ( next_newline_pos != p_script.end() )
			next_newline_pos++;
		// The interval looked into is [first, last ) so we need to increment it
		// in order to include the newline into the line
		const std::regex	line_regex { "([a-zA-Z]+) ([a-zA-Z]+)([0-9]+) (([a-zA-Z0-9]+ )?[0-9a-zA-Z]+)\r?\n?" };
		static const size_t NUM_EXPECTED_GROUPS = 4;
		std::smatch			match;

		while ( current_pos != p_script.end() )
		{
			std::string line_buf {
				current_pos, next_newline_pos
			}; // We need to used this buffer because the regex match function doesn't stop at the en of a string view
			   // iterator but rather at the end of the entire super-string. So we need to copy the string data on a
			   // separated buffer for it to work as intended.
			if ( std::regex_match( line_buf, match, line_regex ) == false || match.size() < NUM_EXPECTED_GROUPS + 1 )
			{
				out.error	= true;
				out.message = std::format(
					"Line <{}> isn't understood.\nPattern is \n[chain] [kw][num] [value]\n",
					std::string( current_pos, next_newline_pos )
				);
				return out;
			}
			InteractiveId new_id;

			new_id.chain	= match[ 1 ].str()[ 0 ];
			std::string buf = match[ 2 ].str();
			Util::String::toUpper( buf );
			parse( buf, new_id.kw );
			if ( new_id.kw == E_INTERACTIVE_KEYWORD::none )
			{
				out.error	= true;
				out.message = std::format(
					"residue <{}> isn't understood.\nCorrect values are HIS, LYS, ASP, GLN, GLU, ARG, TER, SS", buf
				);
				return out;
			}
			buf = match[ 3 ].str();
			if ( buf.empty() )
			{
				out.error	= true;
				out.message = std::format(
					"No residue num has been found in line <{}>.", std::string( current_pos, next_newline_pos )
				);
				return out;
			}
			try
			{
				new_id.num = std::stoul( buf );
			}
			catch ( std::invalid_argument & )
			{
				out.error	= true;
				out.message = std::format( "Number <{}> isn't a correct value.", buf );
				return out;
			}
			buf = match[ 4 ].str();
			if ( buf.empty() )
			{
				out.error	= true;
				out.message = std::format(
					"No value has been found in line <{}>.", std::string( current_pos, next_newline_pos )
				);
				return out;
			}

			p_args.kwValue.insert( { std::move( new_id ), buf } );
			current_pos		 = next_newline_pos;
			next_newline_pos = std::find( current_pos, p_script.end(), '\n' );
			if ( next_newline_pos != p_script.end() )
				next_newline_pos++;
		}

		return out;
	}

} // namespace VTX::Tool::Mdprep::Gromacs
