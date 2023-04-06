#ifndef __VTX_BASE_WORKER__
#define __VTX_BASE_WORKER__

namespace VTX
{
	namespace Worker
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
	} // namespace Worker
} // namespace VTX
#endif
