#include "app/old/internal/worker/program_launcher.hpp"

namespace VTX::App::Old::Internal::Worker
{
	uint ProgramLauncher::_run() { return system( _command.c_str() ); }

} // namespace VTX::App::Old::Internal::Worker
