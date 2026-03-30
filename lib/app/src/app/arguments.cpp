#include "app/arguments.hpp"
#include "app/session.hpp"
#include <argparse/argparse.hpp>
#include <cassert>
#include <iostream>

namespace VTX::App
{

	struct ArgumentParser::_data
	{
		Arguments				 args;
		bool					 args_gone = false;
		bool					 needHelp  = false;
		argparse::ArgumentParser parser { std::string( "VTX" ), Session::version(), argparse::default_arguments::none };
	};
	ArgumentParser::ArgumentParser( int p_argc, const char * const * p_argv ) noexcept :
		_impl( new _data { Arguments { p_argc, p_argv } } )
	{
		_impl->parser.add_description( "Start VTX with command line arguments." );

		// Positional arg
		_impl->parser.add_argument( "files" )
			.help(
				"List of files to submit to VTX. System will be loaded and python script will be executed at startup."
			)
			.metavar( "FILES" )
			.store_into( _impl->args.positionalFiles )
			.nargs( argparse::nargs_pattern::any );

		// Bools
		_impl->parser.add_argument( "-h", "--help" )
			.help( "Print the help message and exit." )
			.default_value( false )
			.implicit_value( true )
			.nargs( 0 )
			.store_into( _impl->needHelp );
		_impl->parser.add_argument( "--debug" )
			.help( "Enable debug mode. This opens a console and print a ton of debug messages." )
			.default_value( false )
			.implicit_value( true )
			.nargs( 0 )
			.store_into( _impl->args.debug );
		_impl->parser.add_argument( "--no-gui" )
			.help( "Turn the gui off. Usefull for executing scripts in batch." )
			.default_value( false )
			.implicit_value( true )
			.nargs( 0 )
			.store_into( _impl->args.noGui );
		_impl->parser.add_argument( "--no-graphics" )
			.help( "Disable the rendering panel. Intended for debug purposes." )
			.default_value( false )
			.implicit_value( true )
			.nargs( 0 )
			.store_into( _impl->args.noGraphics );
		_impl->parser.add_argument( "--no-updates" )
			.help( "Disable automatic updating." )
			.default_value( false )
			.implicit_value( true )
			.nargs( 0 )
			.store_into( _impl->args.noUpdates );
	}
	bool		ArgumentParser::needHelp() const { return _impl->needHelp; }
	std::string ArgumentParser::help() const
	{
		std::stringstream ss;

		ss << _impl->parser << std::endl;
		auto s = ss.str();
		return s;
	}
	void ArgumentParser::get( Arguments & p_args ) noexcept
	{
		assert( _impl->args_gone == false );

		p_args			 = std::move( _impl->args );
		_impl->args_gone = true;
	}
	void ArgumentParser::parse()
	{
		_impl->parser.parse_args( _impl->args.argc, _impl->args.argv );
	}

	std::string toString( const Arguments & p_args ) noexcept
	{
		assert( p_args.argv != nullptr );
		assert( p_args.argc > 0 );
		std::string out( p_args.argv[ 0 ] );

		for ( int i = 0; i < p_args.argc; i++ )
		{
			out += " ";
			out += std::string( p_args.argv[ i ] );
		}
		return out;
	}

	void ArgumentParser::_del::operator()( _data * p_ ) const noexcept { delete p_; }

} // namespace VTX::App
