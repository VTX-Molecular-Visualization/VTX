#ifndef __VTX_UI_ACTION_RENDERER__
#define __VTX_UI_ACTION_RENDERER__

#include <app/core/action/base_action.hpp>
#include <app/old_app/color/rgba.hpp>
#include <app/application/render_effect/render_effect_preset.hpp>

namespace VTX::UI::Action::Renderer
{
	class ChangeShading : public VTX::App::Core::Action::BaseAction
	{
	  public:
		ChangeShading( App::Application::RenderEffect::RenderEffectPreset & p_preset, const VTX::Renderer::SHADING & p_shading ) :
			_preset( p_preset ), _shading( p_shading ) {};

		virtual void execute() override;

	  private:
		App::Application::RenderEffect::RenderEffectPreset & _preset;
		const VTX::Renderer::SHADING		  _shading;
	};

	class EnableSSAO : public VTX::App::Core::Action::BaseAction
	{
	  public:
		EnableSSAO( App::Application::RenderEffect::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		App::Application::RenderEffect::RenderEffectPreset & _preset;
		const bool							  _enable;
	};

	class EnableOutline : public VTX::App::Core::Action::BaseAction
	{
	  public:
		EnableOutline( App::Application::RenderEffect::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		App::Application::RenderEffect::RenderEffectPreset & _preset;
		const bool							  _enable;
	};

	class EnableFog : public VTX::App::Core::Action::BaseAction
	{
	  public:
		EnableFog( App::Application::RenderEffect::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		App::Application::RenderEffect::RenderEffectPreset & _preset;
		const bool							  _enable;
	};

} // namespace VTX::UI::Action::Renderer
#endif
