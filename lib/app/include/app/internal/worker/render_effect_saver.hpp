#ifndef __VTX_WORKER_RENDER_EFFECT_PRESET_SAVER__
#define __VTX_WORKER_RENDER_EFFECT_PRESET_SAVER__

#include "app/application/render_effect/_fwd.hpp"
#include "app/core/worker/base_thread.hpp"
#include "app/core/worker/base_worker.hpp"
#include <util/types.hpp>

namespace VTX::Worker
{
	class RenderEffectPresetSaverThread : public App::Core::Worker::BaseThread
	{
		// Q_OBJECT

	  public:
		explicit RenderEffectPresetSaverThread(
			const App::Application::RenderEffect::RenderEffectPreset * const p_preset ) :
			_preset( p_preset )
		{
		}

	  protected:
		uint _run() override;

	  private:
		const App::Application::RenderEffect::RenderEffectPreset * const _preset;
	};
	class RenderEffectPresetSaver : public VTX::App::Core::Worker::BaseWorker
	{
	  public:
		explicit RenderEffectPresetSaver( const App::Application::RenderEffect::RenderEffectPreset * const p_preset,
										  const FilePath &												   p_path ) :
			_preset( p_preset ),
			_path( p_path )
		{
		}

	  protected:
		void _run() override;

	  private:
		const App::Application::RenderEffect::RenderEffectPreset * const _preset;
		const FilePath													 _path;
	};
}; // namespace VTX::Worker
#endif
