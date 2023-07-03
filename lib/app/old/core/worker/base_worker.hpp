#ifndef __VTX_APP_OLD_CORE_WORKER_BASE_WORKER__
#define __VTX_APP_OLD_CORE_WORKER_BASE_WORKER__

namespace VTX::App::Old::Core::Worker
{
	class BaseWorker
	{
	  public:
		BaseWorker()		  = default;
		virtual ~BaseWorker() = default;

		void run() { _run(); }

	  protected:
		virtual void _run() = 0;
	};
} // namespace VTX::App::Old::Core::Worker
#endif
