#ifndef __VTX_APP_APPLICATION_RENDERER_PROXY_WRAPPER__
#define __VTX_APP_APPLICATION_RENDERER_PROXY_WRAPPER__

#include "app/application/renderer/renderer_accessor.hpp"
#include "app/application/system/renderer.hpp"
#include <memory>

namespace VTX::App::Application::Renderer
{
	template<typename T>
	class ProxyWrapper
	{
	  public:
		class ProxyAccessor
		{
			friend ProxyWrapper;

		  public:
			T & proxy() { return _wrapper.proxy(); }
			~ProxyAccessor() = default;

		  private:
			ProxyAccessor( ProxyWrapper & p_wrapper ) : _wrapper( p_wrapper ), _rendererAccessor( App::RENDERER() ) {}

			Application::Renderer::RendererAccessor _rendererAccessor;
			ProxyWrapper &							_wrapper;
		};

		ProxyWrapper() {}
		ProxyWrapper( std::unique_ptr<T> & p_proxyPtr ) : _proxyPtr( std::move( p_proxyPtr ) ) {}

		void setProxy( std::unique_ptr<T> & p_proxyPtr ) { _proxyPtr = std::move( p_proxyPtr ); }
		bool isValid() { return _proxyPtr != nullptr; }

		ProxyAccessor accessor() { return ProxyAccessor( *this ); }
		T &			  proxy() { return *_proxyPtr; }
		const T &	  proxy() const { return *_proxyPtr; }

		void invalidate() { _proxyPtr = nullptr; }

	  private:
		std::unique_ptr<T> _proxyPtr = nullptr;
	};

} // namespace VTX::App::Application::Renderer
#endif
