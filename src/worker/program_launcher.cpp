#include "program_launcher.hpp"

namespace VTX
{
	namespace Worker
	{
		void ProgramLauncher::work()
		{
			_progress = 0.f;
			system( _command.c_str() );
			_progress = 1.f;
		}

	} // namespace Worker
} // namespace VTX
