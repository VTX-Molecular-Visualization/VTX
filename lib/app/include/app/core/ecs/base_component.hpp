#ifndef __VTX_APP_CORE_ECS_BASE_COMPONENT__
#define __VTX_APP_CORE_ECS_BASE_COMPONENT__

#include <memory>

namespace VTX::App::Core::ECS
{
	class BaseComponent
	{
	  public:
		BaseComponent() {}
		virtual ~BaseComponent() {}
	};

	// TODO: Proxy concept?
	template<typename P>
	class BaseComponentProxy : public BaseComponent
	{
	  public:
		BaseComponentProxy() {}
		virtual ~BaseComponentProxy() {}
		virtual void setupProxy() = 0;

	  protected:
		std::unique_ptr<P> _proxy;
	};
} // namespace VTX::App::Core::ECS

#endif
