#include "program_launcher.hpp"
#include "tool/logger.hpp"

namespace VTX
{
	namespace Worker
	{
		void ProgramLauncher::work()
		{
			VTX_DEBUG( _command );
			_progress = 0.f;
			system( _command.c_str() );
			_progress = 1.f;
		}

	} // namespace Worker
} // namespace VTX
