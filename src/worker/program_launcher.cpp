#include "program_launcher.hpp"
#include "tool/logger.hpp"

namespace VTX
{
	namespace Worker
	{
		uint ProgramLauncher::_run()
		{
			VTX_DEBUG( _command );
			return system( _command.c_str() );
		}

	} // namespace Worker
} // namespace VTX
