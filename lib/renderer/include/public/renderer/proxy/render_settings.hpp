#ifndef __VTX_RENDERER_PROXY_RENDER_SETTINGS__
#define __VTX_RENDERER_PROXY_RENDER_SETTINGS__

#include <renderer/settings.hpp>
#include <util/callback.hpp>
#include <util/collection.hpp>

namespace VTX::Renderer::Proxy
{

	/**
	 * @brief Render settings data provider.
	 */
	class RenderSettings
	{
	  public:
		const VTX::Renderer::RenderSettings & data;

		RenderSettings( const VTX::Renderer::RenderSettings & p_rep ) : data( p_rep ) {}

		template<VTX::Renderer::E_RENDER_SETTINGS S>
		Util::Callback<> & getCallback()
		{
			return *_callbacks.getOrCreateWithHash<Util::Callback<>>( int( S ) );
		}

	  private:
		Util::Collection<std::unique_ptr<Util::Callback<>>> _callbacks;
	};

} // namespace VTX::Renderer::Proxy

#endif
