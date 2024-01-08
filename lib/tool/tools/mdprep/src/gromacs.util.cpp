#include "tools/mdprep/gromacs.util.hpp"
#include "tools/mdprep/gromacs.hpp"
#include <algorithm>
#include <filesystem>
#include <format>
#include <optional>
#include <qapplication.h>
#include <regex>
#include <util/exceptions.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>

namespace VTX::Tool::Mdprep
{
	std::optional<fs::path> g_executable_directory;
	const fs::path &		executable_directory() noexcept
	{
		if ( !g_executable_directory.has_value() )
		{
			// int				 argc = 0;
			// QCoreApplication app( argc, nullptr );
			// QString			 qpath = app.applicationDirPath();
			QString		qpath = QCoreApplication::applicationDirPath();
			auto		tmp	  = qpath.toLocal8Bit();
			std::string path_str( tmp.data(), tmp.size() );
			g_executable_directory.emplace( path_str );
		}
		return g_executable_directory.value();
	}

} // namespace VTX::Tool::Mdprep

namespace VTX::Tool::Mdprep::Gromacs
{
	namespace
	{
		const char g_ff_suffix[] = ".ff";
	}

	const fs::path g_default_ff_directory_relative_path
		= fs::path( "data" ) / "tool" / "tools" / "mdprep" / "gromacs" / "top";
	const fs::path & default_ff_directory_relative_path() noexcept { return g_default_ff_directory_relative_path; }

	void declare_ff_directory( const std::filesystem::path & path ) noexcept
	{
		std::string	   path_str = path.string();
		QByteArrayView env_arg( path_str.data(), path_str.data() + path_str.size() );
		qputenv( "GMXLIB", env_arg );
	}

	std::vector<forcefield> list_forcefields( const fs::path & data_dir )
	{
		if ( !fs::is_directory( data_dir ) )
		{
			auto err_str = std::format( "Directory <{}> not found", data_dir.string() );
			throw VTX::IOException( err_str );
		}
		std::vector<forcefield> out;

		for ( auto & fs_element : fs::directory_iterator( data_dir ) )
		{
			if ( !fs_element.is_directory() )
				continue;

			auto filename = fs_element.path().filename().string();
			if ( filename.ends_with( g_ff_suffix ) )
			{
				out.emplace_back( forcefield { .forcefield_folder_path = fs_element.path().string() } );
				continue;
			}
		}

		return out;
	}

	const char * string( const water_model & w ) noexcept
	{
		switch ( w )
		{
		case VTX::Tool::Mdprep::Gromacs::water_model::none: return "none";
		case VTX::Tool::Mdprep::Gromacs::water_model::spc: return "spc";
		case VTX::Tool::Mdprep::Gromacs::water_model::spce: return "spce";
		case VTX::Tool::Mdprep::Gromacs::water_model::tip3p: return "tip3p";
		case VTX::Tool::Mdprep::Gromacs::water_model::tip4p: return "tip4p";
		case VTX::Tool::Mdprep::Gromacs::water_model::tip5p: return "tip5p";
		case VTX::Tool::Mdprep::Gromacs::water_model::tips3p: return "tips3p";
		default: break;
		}
		return "";
	}

	void convert( const pdb2gmx_instructions & in, gromacs_command_args & out ) noexcept
	{
		if ( in.forcefields.empty() )
			return;
		if ( in.forcefield_index >= in.forcefields.size() )
			return;
		if ( in.input_pdb.empty() )
			return;
		if ( !in.input_pdb.has_filename() )
			return;

		fs::path output_dir = in.output_dir;
		if ( output_dir.empty() )
			output_dir = in.input_pdb.parent_path();
		fs::create_directories( output_dir );
		out.arguments.clear();

		out.arguments.push_back( "pdb2gmx" );
		out.arguments.push_back( "-f" );
		out.arguments.push_back( in.input_pdb.string() );
		std::string input_root_name = in.input_pdb.filename().string();
		out.arguments.push_back( "-o" );
		out.arguments.push_back( ( output_dir / ( input_root_name + ".gro" ) ).string() );
		out.arguments.push_back( "-p" );
		out.arguments.push_back( ( output_dir / ( input_root_name + ".top" ) ).string() );
		out.arguments.push_back( "-i" );
		out.arguments.push_back( ( output_dir / ( input_root_name + ".itp" ) ).string() );
		out.arguments.push_back( "-q" );
		out.arguments.push_back( ( output_dir / ( input_root_name + ".clean.pdb" ) ).string() );
		out.arguments.push_back( "-n" );
		out.arguments.push_back( ( output_dir / ( input_root_name + ".ndx" ) ).string() );
		out.arguments.push_back( "-ff" );
		out.arguments.push_back( in.forcefields.at( in.forcefield_index ).get_name().data() );
		out.arguments.push_back( "-water" );
		out.arguments.push_back( string( in.water ) );
	}
	void convert( const solvate_instructions &, gromacs_command_args & ) noexcept {}

	std::string_view forcefield::get_name() const
	{
		size_t filename_pos
			= this->forcefield_folder_path.find( fs::path( this->forcefield_folder_path ).filename().string() );
		size_t extension_pos = this->forcefield_folder_path.size() - ( sizeof( g_ff_suffix ) - 1 );
		return std::string_view { std::next( this->forcefield_folder_path.begin(), filename_pos ),
								  std::next( this->forcefield_folder_path.begin(), extension_pos ) };
	}

	void parse( const std::string & user_str, interactive_keyword & out ) noexcept
	{
		if ( user_str == "HIS" )
		{
			out = interactive_keyword::his;
			return;
		};
		if ( user_str == "LYS" )
		{
			out = interactive_keyword::lys;
			return;
		};
		if ( user_str == "ASP" )
		{
			out = interactive_keyword::asp;
			return;
		};
		if ( user_str == "GLN" )
		{
			out = interactive_keyword::gln;
			return;
		};
		if ( user_str == "GLU" )
		{
			out = interactive_keyword::glu;
			return;
		};
		if ( user_str == "ARG" )
		{
			out = interactive_keyword::arg;
			return;
		};
		if ( user_str == "TER" )
		{
			out = interactive_keyword::ter;
			return;
		};
		if ( user_str == "SS" )
		{
			out = interactive_keyword::ss;
			return;
		};
		out = interactive_keyword::none;
		return;
	}

	parse_report parse_pdb2gmx_user_script( const std::string_view & script, interactive_arguments & args ) noexcept
	{
		parse_report out;

		args.kw_v.clear();
		if ( script.empty() )
			return out;

		std::string_view::const_iterator current_pos	  = script.begin(),
										 next_newline_pos = ++std::find(
											 script.begin(), script.end(), '\n'
										 ); // The interval looked into is [first, last ) so we need to increment it
											// in order to include the newline into the line
		const std::regex	line_regex { "([a-zA-Z]+) ([a-zA-Z]+)([0-9]+) ([0-9a-zA-Z]+)\n?" };
		static const size_t NUM_EXPECTED_GROUPS = 4;
		std::match_results<std::string_view::const_iterator> match;
		// std::smatch match;

		while ( current_pos != script.end() )
		{
			std::string_view current_view { current_pos, next_newline_pos }; // mais kel enculer
			// std::string aaaaa { current_pos, next_newline_pos };
			// if ( std::regex_search( aaaaa, match, line_regex ) == false || match.size() < NUM_EXPECTED_GROUPS )
			if ( std::regex_search( current_view.begin(), current_view.end(), match, line_regex ) == false
				 || match.size() < NUM_EXPECTED_GROUPS )
			{
				out.error	= true;
				out.message = std::format(
					"Line <{}> isn't understood.\nPattern is \n[chain] [kw][num] [value]\n",
					std::string( current_pos, next_newline_pos )
				);
				return out;
			}
			interactive_id new_id;

			new_id.chain	= match[ 0 ].str()[ 0 ];
			std::string buf = match[ 1 ].str();
			Util::String::toUpper( buf );
			parse( buf, new_id.kw );
			if ( new_id.kw == interactive_keyword::none )
			{
				out.error	= true;
				out.message = std::format(
					"residue <{}> isn't understood.\nCorrect values are HIS, LYS, ASP, GLN, GLU, ARG, TER, SS", buf
				);
				return out;
			}
			buf = match[ 2 ].str();
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
			buf = match[ 3 ].str();
			if ( buf.empty() )
			{
				out.error	= true;
				out.message = std::format(
					"No value has been found in line <{}>.", std::string( current_pos, next_newline_pos )
				);
				return out;
			}

			args.kw_v.insert( { std::move( new_id ), buf } );
			current_pos		 = next_newline_pos;
			next_newline_pos = std::find( current_pos, script.end(), '\n' );
		}

		return out;
	}

} // namespace VTX::Tool::Mdprep::Gromacs
