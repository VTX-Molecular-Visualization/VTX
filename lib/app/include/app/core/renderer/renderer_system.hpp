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
		inline static FilePath SHADER_DIR = Util::Filesystem::getExecutableDir();

		RendererSystem() : VTX::Renderer::Facade( 1, 1, SHADER_DIR ) {}

		/**
		 * @brief Initialize the renderer system, must be called before construction.
		 * @param p_shaderPath the path to the shader directory.
		 */
		inline static void init( const FilePath & p_shaderPath ) { SHADER_DIR = p_shaderPath; }
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
