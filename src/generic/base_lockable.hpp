#ifndef __VTX_BASE_LOCKABLE__
#define __VTX_BASE_LOCKABLE__

#include <QRecursiveMutex>

namespace VTX
{
	namespace Generic
	{
		class BaseLockable
		{
		  protected:
			inline void _lock() { _mutex.lock(); }
			inline void _unlock() { _mutex.unlock(); }

		  private:
			QRecursiveMutex _mutex = QRecursiveMutex();
		};
	} // namespace Generic
} // namespace VTX
#endif
