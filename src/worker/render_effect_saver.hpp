#ifndef __VTX_WORKER_RENDER_EFFECT_PRESET_SAVER__
#define __VTX_WORKER_RENDER_EFFECT_PRESET_SAVER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_thread.hpp"
#include "base_worker.hpp"
#include "define.hpp"

namespace VTX
{
	namespace Model::Renderer
	{
		class RenderEffectPreset;
	} // namespace Model::Renderer

	namespace Worker
	{
		class RenderEffectPresetSaverThread : public Worker::BaseThread
		{
			Q_OBJECT

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
		class RenderEffectPresetSaver : public Worker::BaseWorker
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
