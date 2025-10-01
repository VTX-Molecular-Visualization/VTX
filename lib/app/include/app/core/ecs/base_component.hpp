#ifndef __VTX_APP_CORE_ECS_BASE_COMPONENT__
#define __VTX_APP_CORE_ECS_BASE_COMPONENT__

#include "app/library/base_preset.hpp"
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

	template<typename PRO, Library::ConceptPreset PRE>
	class BaseComponentProxyPreset : public BaseComponentProxy<PRO>
	{
	  public:
		BaseComponentProxyPreset() = delete;
		BaseComponentProxyPreset( PRE & p_preset ) : _preset( p_preset ) {}
		virtual ~BaseComponentProxyPreset() {}

		inline PRE &	   getPreset() { return _preset; }
		inline const PRE & getPreset() const { return _preset; }

	  protected:
		PRE & _preset;
	};
} // namespace VTX::App::Core::ECS

#endif
