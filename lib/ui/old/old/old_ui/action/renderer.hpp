#ifndef __VTX_UI_ACTION_RENDERER__
#define __VTX_UI_ACTION_RENDERER__

#include <app/old/application/render_effect/render_effect_preset.hpp>
#include <app/old/core/action/base_action.hpp>
#include <util/color/rgba.hpp>

namespace VTX::UI::Action::Renderer
{
	class ChangeShading : public VTX::App::Old::Core::Action::BaseAction
	{
	  public:
		ChangeShading( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset,
					   const App::Old::Render::Renderer::SHADING &				p_shading ) :
			_preset( p_preset ),
			_shading( p_shading ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const App::Old::Render::Renderer::SHADING				 _shading;
	};

	class EnableSSAO : public VTX::App::Old::Core::Action::BaseAction
	{
	  public:
		EnableSSAO( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const bool											 _enable;
	};

	class EnableOutline : public VTX::App::Old::Core::Action::BaseAction
	{
	  public:
		EnableOutline( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const bool											 _enable;
	};

	class EnableFog : public VTX::App::Old::Core::Action::BaseAction
	{
	  public:
		EnableFog( App::Old::Application::RenderEffect::RenderEffectPreset & p_preset, const bool p_enable ) :
			_preset( p_preset ), _enable( p_enable ) {};

		virtual void execute() override;

	  private:
		App::Old::Application::RenderEffect::RenderEffectPreset & _preset;
		const bool											 _enable;
	};

} // namespace VTX::UI::Action::Renderer
#endif
