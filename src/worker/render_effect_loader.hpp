#ifndef __VTX_WORKER_RENDER_EFFECT_PRESET_LOADER__
#define __VTX_WORKER_RENDER_EFFECT_PRESET_LOADER__

#include "base_worker.hpp"
#include "define.hpp"
#include "util/filesystem.hpp"
#include <vector>

namespace VTX
{
	namespace Model::Renderer
	{
		class RenderEffectPresetLibrary;
	}

	namespace Worker
	{
		class RenderEffectPresetLibraryLoader : public Worker::BaseWorker
		{
		  public:
			explicit RenderEffectPresetLibraryLoader( Model::Renderer::RenderEffectPresetLibrary & p_library ) :
				RenderEffectPresetLibraryLoader( p_library,
												 IO::FilePath( Util::Filesystem::getRenderEffectPresetsLibraryDir() ) )
			{
			}
			explicit RenderEffectPresetLibraryLoader( Model::Renderer::RenderEffectPresetLibrary & p_library,
													  const IO::FilePath						   p_path ) :
				_path( p_path ),
				_library( p_library )
			{
			}

			inline void activeNotify( const bool p_notify ) { _notify = p_notify; };

		  protected:
			void _run() override;

		  private:
			const IO::FilePath							 _path;
			Model::Renderer::RenderEffectPresetLibrary & _library;
			bool										 _notify = true;
		};

		class RenderEffectPresetLoader : public Worker::BaseWorker
		{
		  public:
			explicit RenderEffectPresetLoader( const IO::FilePath & p_path ) { _paths.emplace_back( p_path ); }
			explicit RenderEffectPresetLoader( const std::vector<IO::FilePath> & p_paths ) : _paths( p_paths ) {}

		  protected:
			void _run() override;

		  private:
			std::vector<IO::FilePath> _paths = std::vector<IO::FilePath>();
		};
	} // namespace Worker
} // namespace VTX
#endif
