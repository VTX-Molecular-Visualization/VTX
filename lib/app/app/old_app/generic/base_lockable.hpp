#ifndef __VTX_BASE_LOCKABLE__
#define __VTX_BASE_LOCKABLE__

// #include <QRecursiveMutex>

// TODO make that without Qt
namespace VTX::Generic
{
	class BaseLockable
	{
	  protected:
		inline void _lock()
		{
			//_mutex.lock();
		}
		inline void _unlock()
		{
			//_mutex.unlock();
		}

	  private:
		// QRecursiveMutex _mutex = QRecursiveMutex();
	};
} // namespace VTX::Generic
#endif
