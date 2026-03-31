#ifndef __VTX_APP_ARGS__
#define __VTX_APP_ARGS__

#include <algorithm>
#include <numeric>
#include <string>
#include <unordered_set>

namespace VTX::App
{
	using Arg = std::string_view;
	// struct Arg
	//{
	//	std::string_view strShort;
	//	std::string_view strLong;
	//	std::string_view helper;

	//	bool operator==( const Arg & ) const noexcept = default;
	//};
	// bool operator==( const Arg & l, const std::string_view & r ) noexcept;

	/**
	 * @brief Store command line arguments.
	 */
	class Args
	{
	  public:
		/**
		 * @brief Construct from argc/argv pair.
		 */
		Args( const int p_argc, const char * const p_argv[] ) : _args( { p_argv, p_argv + p_argc } ) {}

		/**
		 * @brief Construct from set.
		 */
		Args( const std::unordered_set<Arg> & p_args ) : _args( p_args ) {}

		/**
		 * @brief Check argument existence.
		 */
		inline bool has( const std::string_view p_arg ) const { return _args.contains( p_arg ); }

		/**
		 * @brief Add an argument.
		 */
		inline void add( const Arg p_arg ) { _args.emplace( p_arg ); }

		/**
		 * @brief Get all arguments.
		 */
		inline const std::unordered_set<Arg> & all() const { return _args; }

		/**
		 * @brief Convert arguments to a single string.
		 */
		std::string toString() const
		{
			std::string str;
			for ( const auto arg : _args )
			{
				str.append( arg ).append( " " );
			}
			return str;
		}

		/**
		 * @brief Convert arguments to a vector of strings.
		 */
		std::vector<std::string> toStringVec() const
		{
			std::vector<std::string> vec;
			for ( const auto arg : _args )
			{
				vec.emplace_back( arg );
			}
			return vec;
		}

	  private:
		/**
		 * @brief Stored arguments.
		 */
		std::unordered_set<Arg> _args;
	};
} // namespace VTX::App

#endif
