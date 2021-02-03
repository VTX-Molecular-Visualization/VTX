#ifndef __VTX_ACTION_RENDERER__
#define __VTX_ACTION_RENDERER__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "vtx_app.hpp"

namespace VTX::Action::Renderer
{
	class ApplyRenderEffectPreset : public BaseAction
	{
	  public:
		ApplyRenderEffectPreset( const Model::Renderer::RenderEffectPreset & p_preset ) : _preset( p_preset ) {};
		virtual void execute() override
		{
			_preset.apply();
			VTXApp::get().MASK |= VTX_MASK_UNIFORM_UPDATED;
		};

	  private:
		const Model::Renderer::RenderEffectPreset & _preset;
	};
} // namespace VTX::Action::Renderer
#endif
