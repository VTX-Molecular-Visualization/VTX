#ifndef __VTX_APP_CORE_RENDERER_RENDERER_SYSTEM__
#define __VTX_APP_CORE_RENDERER_RENDERER_SYSTEM__

#include <renderer/facade.hpp>
#include <util/filesystem.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Core::Renderer
{
	class RendererSystem : public VTX::Renderer::Facade
	{
	  public:
		RendererSystem() : VTX::Renderer::Facade( 1, 1 ) {}
	};

} // namespace VTX::App::Core::Renderer

namespace VTX::App
{
	inline Core::Renderer::RendererSystem & RENDERER_SYSTEM()
	{
		return Util::Singleton<Core::Renderer::RendererSystem>::get();
	}
} // namespace VTX::App
#endif
