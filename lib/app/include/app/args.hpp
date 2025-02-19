#ifndef __VTX_APP_ARGS__
#define __VTX_APP_ARGS__

#include <numeric>
#include <string>
#include <vector>
#include <algorithm>

namespace VTX::App
{
	using Arg = std::string_view;

	class Args
	{
	  public:
		static inline Arg NO_GRAPHICS { "--no-graphics" };

		Args( const int p_argc, const char * const p_argv[] ) : _args( { p_argv, p_argv + p_argc } ) {}
		Args( const std::vector<Arg> & p_args ) : _args( p_args ) {}

		inline bool has( const Arg p_arg ) const
		{
			return std::find( _args.begin(), _args.end(), p_arg ) != _args.end();
		}

		inline void add( const Arg p_arg ) { _args.push_back( p_arg ); }

		inline const std::vector<Arg> & all() const { return _args; }

		inline std::string toString() const
		{
			std::string str;
			for ( const auto arg : _args )
			{
				str.append( arg ).append( " " );
			}
			return str;
		}

	  private:
		std::vector<Arg> _args;
	};
} // namespace VTX::App

#endif
