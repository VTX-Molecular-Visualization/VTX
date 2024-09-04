#ifndef __VTX_APP_APPLICATION_SYSTEM_RENDERER__
#define __VTX_APP_APPLICATION_SYSTEM_RENDERER__

#include <app/filesystem.hpp>
#include <renderer/facade.hpp>
#include <util/singleton.hpp>

namespace VTX::App::Application::System
{
	class Renderer
	{
	  public:
		Renderer() = default;

		void init()
		{
			const FilePath shaderDir = Filesystem::getShadersDir();
			_facade					 = std::make_unique<VTX::Renderer::Facade>( 1, 1, shaderDir );
		}

		inline VTX::Renderer::Facade & facade() { return *_facade; }

	  private:
		std::unique_ptr<VTX::Renderer::Facade> _facade;
	};

} // namespace VTX::App::Application::System

namespace VTX::App
{
	inline Application::System::Renderer & RENDERER() { return Util::Singleton<Application::System::Renderer>::get(); }
} // namespace VTX::App
#endif
