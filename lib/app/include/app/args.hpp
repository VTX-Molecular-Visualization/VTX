#ifndef __VTX_APP_ARGS__
#define __VTX_APP_ARGS__

#include <algorithm>
#include <numeric>
#include <string>
#include <unordered_set>

namespace VTX::App
{
	using Arg = std::string_view;

	class Args
	{
	  public:
		Args() = default;
		Args( const int p_argc, const char * const p_argv[] ) : _args( { p_argv, p_argv + p_argc } ) {}
		Args( const std::unordered_set<Arg> & p_args ) : _args( p_args ) {}

		inline bool has( const Arg p_arg ) const { return _args.contains( p_arg ); }

		inline void add( const Arg p_arg ) { _args.emplace( p_arg ); }

		inline const std::unordered_set<Arg> & all() const { return _args; }

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
		std::unordered_set<Arg> _args;
	};
} // namespace VTX::App

#endif
