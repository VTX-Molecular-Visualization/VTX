#ifndef __VTX_APP_ARGUMENTS__
#define __VTX_APP_ARGUMENTS__

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace VTX::App
{

	struct Arguments
	{
		int					 argc = 1;
		const char * const * argv = nullptr;

		bool debug		= false;
		bool noGui		= false;
		bool noGraphics = false;
		bool noPython	= false;
		bool noUpdates	= false;

		std::vector<std::string> positionalFiles;
	};

	/**
	 * @brief Class responsible for parsing arguments
	 */
	class ArgumentParser
	{
	  public:
		ArgumentParser( int p_argc, const char * const * p_argv ) noexcept;

		void		parse();
		bool		needHelp() const;
		std::string help() const;
		void		get( Arguments & ) noexcept;

	  private:
		struct _data;

		struct _del
		{
			void operator()( _data * ) const noexcept;
		};

		std::unique_ptr<_data, _del> _impl;
	};

	std::string				 toString( const Arguments & ) noexcept;
	std::vector<std::string> toStringVector( const Arguments & ) noexcept;

} // namespace VTX::App

#endif
