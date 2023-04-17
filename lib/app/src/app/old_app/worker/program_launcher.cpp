#include "app/old_app/worker/program_launcher.hpp"

namespace VTX
{
	namespace Worker
	{
		uint ProgramLauncher::_run() { return system( _command.c_str() ); }

	} // namespace Worker
} // namespace VTX
