#ifndef __VTX_APP_APPLICATION_SYSTEM_RENDERER__
#define __VTX_APP_APPLICATION_SYSTEM_RENDERER__

#include "app/application/system/system_registration.hpp"
#include <app/filesystem.hpp>
#include <renderer/facade.hpp>

namespace VTX::App::Application::System
{
	class Renderer : public System::AutoRegistrateSystem<Renderer>
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
	Application::System::Renderer & RENDERER();
} // namespace VTX::App
#endif
