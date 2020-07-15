#ifndef __VTX_PROGRAM_LAUNCHER__
#define __VTX_PROGRAM_LAUNCHER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_worker.hpp"

namespace VTX
{
	namespace Worker
	{
		class ProgramLauncher : public Worker::BaseWorker
		{
		  public:
			explicit ProgramLauncher( const std::string & p_command ) : _command( p_command ) {}

			virtual void work() override;

		  private:
			std::string _command;
		};
	} // namespace Worker
} // namespace VTX
#endif
