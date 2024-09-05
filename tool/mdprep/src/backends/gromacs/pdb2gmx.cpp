#include <fstream>
#include <qprocess.h>
#include <re2/re2.h>
#include <regex>
#include <set>
//
#include <tool/mdprep/backends/gromacs/pdb2gmx.hpp>
#include <util/exceptions.hpp>
#include <util/string.hpp>
//
#include <tool/mdprep/backends/gromacs/inputs.hpp>
// impl should come before inputs
#include "tool/mdprep/backends/gromacs/job.hpp"
#include <tool/mdprep/backends/gromacs/pdb2gmx.impl.hpp>
#include <tool/mdprep/backends/gromacs/util.hpp>
namespace VTX::Tool::Mdprep::backends::Gromacs
{

	namespace
	{
		const char g_ff_suffix[] = ".ff";
	}

	std::vector<forcefield> listForcefields( const fs::path & p_dataDir )
	{
		if ( !fs::is_directory( p_dataDir ) )
		{
			auto errStr = fmt::format( "Directory <{}> not found", p_dataDir.string() );
			throw VTX::IOException( errStr );
		}
		std::vector<forcefield> out;

		for ( auto & fsElement : fs::directory_iterator( p_dataDir ) )
		{
			if ( !fsElement.is_directory() )
				continue;

			auto filename = fsElement.path().filename().string();
			if ( filename.ends_with( g_ff_suffix ) )
			{
				out.emplace_back( forcefield { .forcefieldFolderPath = fsElement.path().string() } );
				continue;
			}
		}

		return out;
	}

	const char * string( const E_WATER_MODEL & p_w ) noexcept
	{
		switch ( p_w )
		{
		case VTX::Tool::Mdprep::backends::Gromacs::E_WATER_MODEL::none: return "none";
		case VTX::Tool::Mdprep::backends::Gromacs::E_WATER_MODEL::spc: return "spc";
		case VTX::Tool::Mdprep::backends::Gromacs::E_WATER_MODEL::spce: return "spce";
		case VTX::Tool::Mdprep::backends::Gromacs::E_WATER_MODEL::tip3p: return "tip3p";
		case VTX::Tool::Mdprep::backends::Gromacs::E_WATER_MODEL::tip4p: return "tip4p";
		case VTX::Tool::Mdprep::backends::Gromacs::E_WATER_MODEL::tip5p: return "tip5p";
		case VTX::Tool::Mdprep::backends::Gromacs::E_WATER_MODEL::tips3p: return "tips3p";
		default: break;
		}
		return "";
	}
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
		size_t filenamePos
			= this->forcefieldFolderPath.find( fs::path( this->forcefieldFolderPath ).filename().string() );
		size_t extensionPos = this->forcefieldFolderPath.size() - ( sizeof( g_ff_suffix ) - 1 );
		return std::string_view { std::next( this->forcefieldFolderPath.begin(), filenamePos ),
								  std::next( this->forcefieldFolderPath.begin(), extensionPos ) };
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

	parseReport parsePdb2gmxUserScript( const std::string_view & p_script, Pdb2gmxInputs & p_args ) noexcept
	{
		parseReport out;

		p_args.kwValue.clear();
		if ( p_script.empty() )
			return out;

		std::string_view::const_iterator currentPos		= p_script.begin(),
										 nextNewlinePos = std::find( p_script.begin(), p_script.end(), '\n' );
		if ( nextNewlinePos != p_script.end() )
			nextNewlinePos++;
		// The interval looked into is [first, last ) so we need to increment it
		// in order to include the newline into the line
		const std::regex lineRegex { "([a-zA-Z]+) ([a-zA-Z]+)([0-9]+) (([a-zA-Z0-9]+ )?[0-9a-zA-Z]+)\r?\n?" };
		const size_t	 NUM_EXPECTED_GROUPS = 4;
		std::smatch		 match;

		while ( currentPos != p_script.end() )
		{
			std::string lineBuf {
				currentPos, nextNewlinePos
			}; // We need to used this buffer because the regex match function doesn't stop at the en of a string view
			   // iterator but rather at the end of the entire super-string. So we need to copy the string data on a
			   // separated buffer for it to work as intended.
			if ( std::regex_match( lineBuf, match, lineRegex ) == false || match.size() < NUM_EXPECTED_GROUPS + 1 )
			{
				out.error	= true;
				out.message = fmt::format(
					"Line <{}> isn't understood.\nPattern is \n[chain] [kw][num] [value]\n",
					std::string( currentPos, nextNewlinePos )
				);
				return out;
			}
			Pdb2gmxInputId new_id;

			new_id.chain	= match[ 1 ].str()[ 0 ];
			std::string buf = match[ 2 ].str();
			Util::String::toUpper( buf );
			parse( buf, new_id.kw );
			if ( new_id.kw == E_INTERACTIVE_KEYWORD::none )
			{
				out.error	= true;
				out.message = fmt::format(
					"residue <{}> isn't understood.\nCorrect values are HIS, LYS, ASP, GLN, GLU, ARG, TER, SS", buf
				);
				return out;
			}
			buf = match[ 3 ].str();
			if ( buf.empty() )
			{
				out.error	= true;
				out.message = fmt::format(
					"No residue num has been found in line <{}>.", std::string( currentPos, nextNewlinePos )
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
				out.message = fmt::format( "Number <{}> isn't a correct value.", buf );
				return out;
			}
			buf = match[ 4 ].str();
			if ( buf.empty() )
			{
				out.error = true;
				out.message
					= fmt::format( "No value has been found in line <{}>.", std::string( currentPos, nextNewlinePos ) );
				return out;
			}

			p_args.kwValue.insert( { std::move( new_id ), buf } );
			currentPos	   = nextNewlinePos;
			nextNewlinePos = std::find( currentPos, p_script.end(), '\n' );
			if ( nextNewlinePos != p_script.end() )
				nextNewlinePos++;
		}

		return out;
	}
	void prepareJob(
		const CumulativeOuputFiles &,
		const fs::path &		 p_root,
		const std::string_view & p_folderName,
		Pdb2gmxInstructions &	 p_instructions
	) noexcept
	{
		if ( fs::exists( p_root ) && fs::is_directory( p_root ) == false )
		{
			return; // This scenario shouldn't happen
		}
		fs::path jobDir = p_root / p_folderName;
		fs::create_directories( jobDir );

		p_instructions.outputDir = jobDir.string();
	}
	namespace
	{
		void postJobRoutine( const fs::path & p_jobDir, GromacsJobData & p_jobData, CumulativeOuputFiles & p_outputs )
		{
			// The issue here is the .itp file. When the input structure has multiple chain, multiple itp files are
			// created and referenced into the .top file using relative path.
			// So we are going to edit the top file and replace any relative path file that start with our fileStem by
			// their absolute path version. This will cover both multi-chain and single chain scenarios, and maybe even
			// some scenarios that we didn't anticipate
			fs::path topFile;
			for ( auto & outputFileIdx : p_jobData.expectedOutputFilesIndexes )
			{
				auto & fileStr = p_jobData.arguments.at( outputFileIdx );
				if ( fileStr.ends_with( ".top" ) )
				{
					topFile = fileStr;
					break;
				}
			}
			if ( topFile.empty() || fs::exists( topFile ) == false )
			{
				return; // if no top file is found it's the checkRslt's responsibility to report it
			}
			fs::path tmpFile = fs::temp_directory_path() / "vtxgmxtopfiletmp";
			if ( fs::exists( tmpFile ) )
				fs::remove( tmpFile );
			{
				std::string fileStem = topFile.stem().string();
				RE2			itpÎncludePattern { fmt::format( "#include +(\"|<)({}.*\\.\\w\\w\\w)(\"|>).*", fileStem ) };
				std::ifstream fileStrm { topFile };
				std::ofstream outStrm { tmpFile };
				const size_t  bufSize	   = 500; // should be way too much for a single line
				std::string	  jobDirString = p_jobDir.string();
				for ( std::array<char, bufSize> buf; fileStrm.getline( buf.data(), bufSize ); )
				{
					std::string fileName;

					if ( RE2::FullMatch( { buf.data() }, itpÎncludePattern, nullptr, &fileName, nullptr ) )
					{
						char * cur = strstr( buf.data(), fileName.data() );
						if ( cur >= buf.data() && cur <= ( buf.data() + bufSize ) )
						{
							strcpy_s( cur, bufSize - ( cur - buf.data() ), jobDirString.data() );
							cur += jobDirString.size();
							strcpy_s( cur, bufSize - ( cur - buf.data() ), "/" );
							cur += 1;
							strcpy_s( cur, bufSize - ( cur - buf.data() ), fileName.data() );
							cur += fileName.size();
							strcpy_s( cur, bufSize - ( cur - buf.data() ), "\"" );
						}
					}

					outStrm << buf.data() << std::endl;
				}
			}
			fs::remove( topFile );
			fs::rename( tmpFile, topFile );
		}
	} // namespace
	void convert( const Pdb2gmxInstructions & p_in, GromacsJobData & p_out ) noexcept
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
		p_out.arguments.push_back( "-o" );
		p_out.arguments.push_back( ( outputDir / ( p_in.fileStem + ".gro" ) ).make_preferred().string() );
		setLastArgumentAsExpectedOutputFile( p_out );
		p_out.arguments.push_back( "-p" );
		p_out.arguments.push_back( ( outputDir / ( p_in.fileStem + ".top" ) ).make_preferred().string() );
		setLastArgumentAsExpectedOutputFile( p_out );
		p_out.arguments.push_back( "-i" );
		p_out.arguments.push_back( ( outputDir / ( p_in.fileStem + ".itp" ) ).make_preferred().string() );
		// .itp won't be among the expected output as mulitple files will be produced by multiple chains, giving an
		// "unpredictible" result. Also the file is not usefull for us so far
		p_out.arguments.push_back( "-n" );
		p_out.arguments.push_back( ( outputDir / ( p_in.fileStem + ".ndx" ) ).make_preferred().string() );
		setLastArgumentAsExpectedOutputFile( p_out );
		p_out.arguments.push_back( "-ff" );
		auto ffName = p_in.forcefields.at( p_in.forcefieldIndex ).getName();
		p_out.arguments.push_back( std::string( ffName.data(), ffName.size() ) );
		p_out.arguments.push_back( "-water" );
		p_out.arguments.push_back( string( p_in.water ) );

		if ( p_in.customParameter.has_value() )
		{
			std::set<E_INTERACTIVE_KEYWORD> seen_kw;
			p_out.interactiveSettings.emplace( Pdb2gmxInputs( *p_in.customParameter ) );
			for ( auto & it : p_in.customParameter->kwValue )
			{
				if ( seen_kw.contains( it.first.kw ) )
					continue;

				p_out.arguments.push_back( std::string( "-" ) += string( it.first.kw ) );
			}
		}

		p_out.postJobRoutine = &postJobRoutine;
	}
	bool isWaitingForInput( const Pdb2gmxInputs &, const std::string_view & p_stdout ) noexcept
	{
		return isWaitingInputs( p_stdout );
	}

	bool enterInput(
		const Pdb2gmxInputs & p_inputs,
		QProcess &			  p_proc,
		std::string &		  p_stdout,
		std::string &		  p_stderr
	) noexcept
	{
		Pdb2gmxInputId expectedId;
		bool		   noProblemFoundWhileParsingGromacsOutput = parseExpectedKwArgument( p_stdout, expectedId );
		if ( noProblemFoundWhileParsingGromacsOutput == false )
		{
			p_proc.kill();
			p_stderr += "\nVTX error -- VTX didn't recognized gromacs expected value.";
			return false;
		}

		const char * value = nullptr;
		if ( p_inputs.kwValue.contains( expectedId ) )
			value = p_inputs.kwValue.at( expectedId ).data();
		else
			value = getDefaultValue( expectedId.kw );
		std::string bytesForGromacs { value };
		bytesForGromacs += '\n';
		p_proc.write( bytesForGromacs.data() );
		p_stdout += bytesForGromacs;
		p_proc.waitForBytesWritten();

		return true;
	}
} // namespace VTX::Tool::Mdprep::backends::Gromacs
