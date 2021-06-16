#ifndef __VTX_PROGRAM_LAUNCHER__
#define __VTX_PROGRAM_LAUNCHER__

#include "base_thread.hpp"

namespace VTX
{
	namespace Worker
	{
		class ProgramLauncher : public Worker::BaseThread
		{
			Q_OBJECT

		  public:
			explicit ProgramLauncher( const std::string & p_command ) : _command( p_command ) {}

		  protected:
			uint _run() override;

		  private:
			std::string _command;
		};
	} // namespace Worker
} // namespace VTX
#endif
