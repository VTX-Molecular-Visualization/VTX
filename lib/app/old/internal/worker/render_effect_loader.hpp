#ifndef __VTX_APP_OLD_INTERNAL_WORKER_RENDER_EFFECT_PRESET_LOADER__
#define __VTX_APP_OLD_INTERNAL_WORKER_RENDER_EFFECT_PRESET_LOADER__

#include "app/old/application/render_effect/_fwd.hpp"
#include "app/old/core/worker/base_worker.hpp"
#include "app/old/internal/io/filesystem.hpp"
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Old::Internal::Worker
{
	class RenderEffectPresetLibraryLoader : public App::Old::Core::Worker::BaseWorker
	{
	  public:
		explicit RenderEffectPresetLibraryLoader( App::Old::Application::RenderEffect::RenderEffectLibrary & p_library ) :
			RenderEffectPresetLibraryLoader(
				p_library,
				FilePath( App::Old::Internal::IO::Filesystem::getRenderEffectPresetsLibraryDir() ) )
		{
		}
		explicit RenderEffectPresetLibraryLoader( App::Old::Application::RenderEffect::RenderEffectLibrary & p_library,
												  const FilePath										p_path ) :
			_path( p_path ),
			_library( p_library )
		{
		}

		inline void activeNotify( const bool p_notify ) { _notify = p_notify; };

	  protected:
		void _run() override;

	  private:
		const FilePath										  _path;
		App::Old::Application::RenderEffect::RenderEffectLibrary & _library;
		bool												  _notify = true;
	};

	class RenderEffectPresetLoader : public VTX::App::Old::Core::Worker::BaseWorker
	{
	  public:
		explicit RenderEffectPresetLoader( const FilePath & p_path ) { _paths.emplace_back( p_path ); }
		explicit RenderEffectPresetLoader( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}

	  protected:
		void _run() override;

	  private:
		std::vector<FilePath> _paths = std::vector<FilePath>();
	};
} // namespace VTX::App::Old::Internal::Worker
#endif
