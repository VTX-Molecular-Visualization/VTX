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
			g_executable_directory->make_preferred();
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
		= ( fs::path( "data" ) / "tools" / "mdprep" / "gromacs" / "top" ).make_preferred();
	const fs::path & default_ff_directory_relative_path() noexcept { return g_default_ff_directory_relative_path; }

	const fs::path g_default_gmx_binary_relative_path
		= ( fs::path( "external" ) / "tools" / "mdprep" / "gromacs" / "gmx.exe" ).make_preferred();
	const fs::path & default_gmx_binary_relative_path() noexcept { return g_default_gmx_binary_relative_path; }

	void declare_ff_directory( const std::filesystem::path & p_path ) noexcept
	{
		std::string	   path_str = p_path.string();
		QByteArrayView env_arg( path_str.data(), path_str.data() + path_str.size() );
		qputenv( "GMXLIB", env_arg );
	}

	std::vector<forcefield> list_forcefields( const fs::path & p_data_dir )
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
				out.emplace_back( forcefield { .forcefield_folder_path = fs_element.path().string() } );
				continue;
			}
		}

		return out;
	}

	const char * string( const water_model & p_w ) noexcept
	{
		switch ( p_w )
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

	void convert( const pdb2gmx_instructions & p_in, GromacsCommandArgs & p_out ) noexcept
	{
		if ( p_in.forcefields.empty() )
			return;
		if ( p_in.forcefield_index >= p_in.forcefields.size() )
			return;
		if ( p_in.input_pdb.empty() )
			return;
		if ( !p_in.input_pdb.has_filename() )
			return;

		fs::path output_dir = p_in.output_dir;
		if ( output_dir.empty() )
			output_dir = p_in.input_pdb.parent_path();
		fs::create_directories( output_dir );
		p_out.arguments.clear();

		p_out.arguments.push_back( "pdb2gmx" );
		p_out.arguments.push_back( "-f" );
		p_out.arguments.push_back( p_in.input_pdb.string() );
		std::string input_root_name = p_in.input_pdb.filename().string();
		p_out.arguments.push_back( "-o" );
		p_out.arguments.push_back( ( output_dir / ( input_root_name + ".gro" ) ).string() );
		p_out.arguments.push_back( "-p" );
		p_out.arguments.push_back( ( output_dir / ( input_root_name + ".top" ) ).string() );
		p_out.arguments.push_back( "-i" );
		p_out.arguments.push_back( ( output_dir / ( input_root_name + ".itp" ) ).string() );
		p_out.arguments.push_back( "-q" );
		p_out.arguments.push_back( ( output_dir / ( input_root_name + ".clean.pdb" ) ).string() );
		p_out.arguments.push_back( "-n" );
		p_out.arguments.push_back( ( output_dir / ( input_root_name + ".ndx" ) ).string() );
		p_out.arguments.push_back( "-ff" );
		p_out.arguments.push_back( p_in.forcefields.at( p_in.forcefield_index ).get_name().data() );
		p_out.arguments.push_back( "-water" );
		p_out.arguments.push_back( string( p_in.water ) );

		if ( p_in.custom_parameter.has_value() )
		{
			std::set<interactive_keyword> seen_kw;
			p_out.interactive_settings = p_in.custom_parameter;
			for ( auto & it : p_in.custom_parameter->kw_v )
			{
				if ( seen_kw.contains( it.first.kw ) )
					continue;

				p_out.arguments.push_back( std::string( "-" ) += string( it.first.kw ) );
			}
		}
	}
	void convert( const solvate_instructions &, GromacsCommandArgs & ) noexcept {}

	const char * string( const interactive_keyword & p_kw ) noexcept
	{
		switch ( p_kw )
		{
		case interactive_keyword::ss: return "ss";
		case interactive_keyword::ter: return "ter";
		case interactive_keyword::lys: return "lys";
		case interactive_keyword::arg: return "arg";
		case interactive_keyword::asp: return "asp";
		case interactive_keyword::glu: return "glu";
		case interactive_keyword::gln: return "gln";
		case interactive_keyword::his: return "his";
		default: return "";
		}
		return "";
	}

	std::string_view forcefield::get_name() const
	{
		size_t filename_pos
			= this->forcefield_folder_path.find( fs::path( this->forcefield_folder_path ).filename().string() );
		size_t extension_pos = this->forcefield_folder_path.size() - ( sizeof( g_ff_suffix ) - 1 );
		return std::string_view { std::next( this->forcefield_folder_path.begin(), filename_pos ),
								  std::next( this->forcefield_folder_path.begin(), extension_pos ) };
	}

	void parse( const std::string & p_user_str, interactive_keyword & p_out ) noexcept
	{
		if ( p_user_str == "HIS" )
		{
			p_out = interactive_keyword::his;
			return;
		};
		if ( p_user_str == "LYS" )
		{
			p_out = interactive_keyword::lys;
			return;
		};
		if ( p_user_str == "ASP" )
		{
			p_out = interactive_keyword::asp;
			return;
		};
		if ( p_user_str == "GLN" )
		{
			p_out = interactive_keyword::gln;
			return;
		};
		if ( p_user_str == "GLU" )
		{
			p_out = interactive_keyword::glu;
			return;
		};
		if ( p_user_str == "ARG" )
		{
			p_out = interactive_keyword::arg;
			return;
		};
		if ( p_user_str == "TER" )
		{
			p_out = interactive_keyword::ter;
			return;
		};
		if ( p_user_str == "SS" )
		{
			p_out = interactive_keyword::ss;
			return;
		};
		p_out = interactive_keyword::none;
		return;
	}

	parse_report parse_pdb2gmx_user_script( const std::string_view & p_script, InteractiveArguments & p_args ) noexcept
	{
		parse_report out;

		p_args.kw_v.clear();
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
			if ( new_id.kw == interactive_keyword::none )
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

			p_args.kw_v.insert( { std::move( new_id ), buf } );
			current_pos		 = next_newline_pos;
			next_newline_pos = std::find( current_pos, p_script.end(), '\n' );
			if ( next_newline_pos != p_script.end() )
				next_newline_pos++;
		}

		return out;
	}

} // namespace VTX::Tool::Mdprep::Gromacs
