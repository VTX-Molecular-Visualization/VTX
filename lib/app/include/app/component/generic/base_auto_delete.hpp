#ifndef __VTX_APP_COMPONENT_GENERIC_BASE_AUTO_DELETE__
#define __VTX_APP_COMPONENT_GENERIC_BASE_AUTO_DELETE__

namespace VTX::App::Component::Generic
{
	class BaseAutoDelete
	{
	  public:
		inline virtual void autoDelete() const = 0;
	};
} // namespace VTX::App::Component::Generic
#endif
