#ifndef __VTX_APP_CORE_RENDERER_PROXY_WRAPPER__
#define __VTX_APP_CORE_RENDERER_PROXY_WRAPPER__

#include "app/core/renderer/renderer_system.hpp"
#include <memory>

namespace VTX::App::Core::Renderer
{
	template<typename T>
	class ProxyWrapper
	{
	  public:
		ProxyWrapper() {}
		ProxyWrapper( std::unique_ptr<T> & p_proxyPtr ) : _proxyPtr( std::move( p_proxyPtr ) ) {}

		void setProxy( std::unique_ptr<T> & p_proxyPtr ) { _proxyPtr = std::move( p_proxyPtr ); }
		bool isValid() { return _proxyPtr != nullptr; }

		T &		  proxy() { return *_proxyPtr; }
		const T & proxy() const { return *_proxyPtr; }

		void invalidate() { _proxyPtr = nullptr; }

	  private:
		std::unique_ptr<T> _proxyPtr = nullptr;
	};

} // namespace VTX::App::Core::Renderer
#endif
