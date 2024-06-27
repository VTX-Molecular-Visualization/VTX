#ifndef ___VTX_APP_ARGS___
#define ___VTX_APP_ARGS___

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

	  private:
		std::vector<std::string> _args;
	};
} // namespace VTX::App

#endif
