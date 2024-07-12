#ifndef __VTX_APP_ARGS__
#define __VTX_APP_ARGS__

#include <numeric>
#include <string>
#include <vector>

namespace VTX::App
{
	class Args
	{
	  public:
		Args( const int p_argc, const char * const p_argv[] ) : _args( { p_argv, p_argv + p_argc } ) {}

		inline bool has( const std::string & p_arg ) const
		{
			return std::find( _args.begin(), _args.end(), p_arg ) != _args.end();
		}

		inline void add( const std::string & p_arg ) { _args.push_back( p_arg ); }

		inline const std::vector<std::string> & all() const { return _args; }

		inline std::string toString() const
		{
			std::string str;
			for ( const auto & arg : _args )
			{
				str += arg + " ";
			}
			return str;
		}

	  private:
		std::vector<std::string> _args;
	};
} // namespace VTX::App

#endif
