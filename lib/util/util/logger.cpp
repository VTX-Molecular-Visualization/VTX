#include <iostream>
#include <logger.hpp>

namespace VTX::Util::Logger
{
	void init( const std::filesystem::path & p_logDir )
	{
		try
		{
			spdlog::set_level( spdlog::level::debug );
			spdlog::set_pattern( "[%t][%H:%M:%S][%^%l%$] %v " );
		}
		catch ( const spdlog::spdlog_ex & p_e )
		{
			std::cout << "Logger initialization failed: " << p_e.what() << std::endl;
		}
	}
} // namespace VTX::Util::Logger
