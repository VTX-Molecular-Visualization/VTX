#ifndef __VTX_BASE_AUTO_DELETE__
#define __VTX_BASE_AUTO_DELETE__

namespace VTX
{
	namespace Generic
	{
		class BaseAutoDelete
		{
		  public:
			inline virtual void autoDelete() const = 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
