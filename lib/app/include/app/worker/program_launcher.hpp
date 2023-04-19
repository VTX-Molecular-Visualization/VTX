#ifndef __VTX_PROGRAM_LAUNCHER__
#define __VTX_PROGRAM_LAUNCHER__

#include "app/core/worker/base_thread.hpp"
#include <string>
#include <util/types.hpp>

namespace VTX::Worker
{
	class ProgramLauncher : public Core::Worker::BaseThread
	{
		// Q_OBJECT

	  public:
		explicit ProgramLauncher( const std::string & p_command ) : _command( p_command ) {}

	  protected:
		uint _run() override;

	  private:
		std::string _command;
	};
} // namespace VTX::Worker
#endif
