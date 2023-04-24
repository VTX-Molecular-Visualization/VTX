#ifndef __VTX_WORKER_RENDER_EFFECT_PRESET_SAVER__
#define __VTX_WORKER_RENDER_EFFECT_PRESET_SAVER__

#include "app/core/worker/base_thread.hpp"
#include "app/core/worker/base_worker.hpp"
#include <util/types.hpp>

namespace VTX
{
	namespace Model::Renderer
	{
		class RenderEffectPreset;
	} // namespace Model::Renderer

	namespace Worker
	{
		class RenderEffectPresetSaverThread : public Core::Worker::BaseThread
		{
			// Q_OBJECT

		  public:
			explicit RenderEffectPresetSaverThread( const Model::Renderer::RenderEffectPreset * const p_preset ) :
				_preset( p_preset )
			{
			}

		  protected:
			uint _run() override;

		  private:
			const Model::Renderer::RenderEffectPreset * const _preset;
		};
		class RenderEffectPresetSaver : public VTX::Core::Worker::BaseWorker
		{
		  public:
			explicit RenderEffectPresetSaver( const Model::Renderer::RenderEffectPreset * const p_preset,
											  const FilePath &									p_path ) :
				_preset( p_preset ),
				_path( p_path )
			{
			}

		  protected:
			void _run() override;

		  private:
			const Model::Renderer::RenderEffectPreset * const _preset;
			const FilePath									  _path;
		};
	}; // namespace Worker
} // namespace VTX
#endif