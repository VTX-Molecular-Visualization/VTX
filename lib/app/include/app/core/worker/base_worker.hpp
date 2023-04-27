#ifndef __VTX_APP_CORE_WORKER_BASE_WORKER__
#define __VTX_APP_CORE_WORKER_BASE_WORKER__

namespace VTX::Core::Worker
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
} // namespace VTX::Core::Worker
#endif
