#include "app/internal/worker/program_launcher.hpp"

namespace VTX::App::Internal::Worker
{
	uint ProgramLauncher::_run() { return system( _command.c_str() ); }

} // namespace VTX::App::Internal::Worker
