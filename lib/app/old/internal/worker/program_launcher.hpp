#ifndef __VTX_APP_OLD_INTERNAL_WORKER_PROGRAM_LAUNCHER__
#define __VTX_APP_OLD_INTERNAL_WORKER_PROGRAM_LAUNCHER__

#include "app/old/core/worker/base_thread.hpp"
#include <string>
#include <util/types.hpp>

namespace VTX::App::Old::Internal::Worker
{
	class ProgramLauncher : public App::Old::Core::Worker::BaseThread
	{
		// Q_OBJECT

	  public:
		explicit ProgramLauncher( const std::string & p_command ) : _command( p_command ) {}

	  protected:
		uint _run() override;

	  private:
		std::string _command;
	};
} // namespace VTX::App::Old::Internal::Worker
#endif
