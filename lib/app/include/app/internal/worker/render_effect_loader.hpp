#ifndef __VTX_WORKER_RENDER_EFFECT_PRESET_LOADER__
#define __VTX_WORKER_RENDER_EFFECT_PRESET_LOADER__

#include "app/core/worker/base_worker.hpp"
#include "app/old_app/io/filesystem.hpp"
#include <util/types.hpp>
#include <vector>

namespace VTX
{
	namespace Model::Renderer
	{
		class RenderEffectPresetLibrary;
	}

	namespace Worker
	{
		class RenderEffectPresetLibraryLoader : public App::Core::Worker::BaseWorker
		{
		  public:
			explicit RenderEffectPresetLibraryLoader( Model::Renderer::RenderEffectPresetLibrary & p_library ) :
				RenderEffectPresetLibraryLoader( p_library,
												 FilePath( IO::Filesystem::getRenderEffectPresetsLibraryDir() ) )
			{
			}
			explicit RenderEffectPresetLibraryLoader( Model::Renderer::RenderEffectPresetLibrary & p_library,
													  const FilePath							   p_path ) :
				_path( p_path ),
				_library( p_library )
			{
			}

			inline void activeNotify( const bool p_notify ) { _notify = p_notify; };

		  protected:
			void _run() override;

		  private:
			const FilePath								 _path;
			Model::Renderer::RenderEffectPresetLibrary & _library;
			bool										 _notify = true;
		};

		class RenderEffectPresetLoader : public VTX::App::Core::Worker::BaseWorker
		{
		  public:
			explicit RenderEffectPresetLoader( const FilePath & p_path ) { _paths.emplace_back( p_path ); }
			explicit RenderEffectPresetLoader( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}

		  protected:
			void _run() override;

		  private:
			std::vector<FilePath> _paths = std::vector<FilePath>();
		};
	} // namespace Worker
} // namespace VTX
#endif
