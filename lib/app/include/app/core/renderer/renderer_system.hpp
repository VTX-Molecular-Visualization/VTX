#ifndef __VTX_APP_CORE_RENDERER_RENDERER_SYSTEM__
#define __VTX_APP_CORE_RENDERER_RENDERER_SYSTEM__

#include <app/filesystem.hpp>
#include <renderer/facade.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Core::Renderer
{
	class RendererSystem
	{
	  public:
		RendererSystem() = default;

		void init()
		{
			const FilePath shaderDir = Filesystem::getShadersDir();
			_facade					 = std::make_unique<VTX::Renderer::Facade>( 1, 1, shaderDir );
		}

		inline VTX::Renderer::Facade & facade() { return *_facade; }

	  private:
		std::unique_ptr<VTX::Renderer::Facade> _facade;
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
