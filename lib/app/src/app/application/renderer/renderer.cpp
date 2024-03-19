#include "app/application/renderer/renderer.hpp"
#include <util/filesystem.hpp>

namespace VTX::App::Application::Renderer
{
	void Renderer::init()
	{
		const FilePath shaderDir = Util::Filesystem::getExecutableDir() / "shaders";
		_rendererPtr			 = std::make_unique<VTX::Renderer::Facade>( 1920, 1080, shaderDir );
	}

	VTX::Renderer::Facade &		  Renderer::get() { return *_rendererPtr; }
	const VTX::Renderer::Facade & Renderer::get() const { return *_rendererPtr; }

} // namespace VTX::App::Application::Renderer
