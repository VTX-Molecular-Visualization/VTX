#ifndef __VTX_UI_ACTION_RENDERER__
#define __VTX_UI_ACTION_RENDERER__

#include "color/rgba.hpp"
#include <old/action/base_action.hpp>
#include <old/model/renderer/render_effect_preset.hpp>

namespace VTX::UI::Action::Renderer
{
	class ChangeShading : public VTX::Action::BaseAction
	{
	  public:
		ChangeShading( Model::Renderer::RenderEffectPreset & p_preset, const VTX::Renderer::SHADING & p_shading ) :
			_preset( p_preset ), _shading( p_shading ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const VTX::Renderer::SHADING		  _shading;
	};

	class EnableSSAO : public VTX::Action::BaseAction
	{
	  public:
		EnableSSAO( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};

	class EnableOutline : public VTX::Action::BaseAction
	{
	  public:
		EnableOutline( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};

	class EnableFog : public VTX::Action::BaseAction
	{
	  public:
		EnableFog( Model::Renderer::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		Model::Renderer::RenderEffectPreset & _preset;
		const bool							  _enable;
	};

} // namespace VTX::UI::Action::Renderer
#endif
