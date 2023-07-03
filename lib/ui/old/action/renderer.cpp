#include "ui/old_ui/action/renderer.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include <app/old/action/renderer.hpp>

namespace VTX::UI::Action::Renderer
{
	void ChangeShading::execute()
	{
		VTX_ACTION( new App::Old::Action::Renderer::ChangeShading( _preset, _shading ) );

		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			QT::QT_APP()->getMainWindow().updateRenderSetting( App::Old::Render::Renderer::RENDER_SETTING::SHADING );
		}
	};

	void EnableSSAO::execute()
	{
		VTX_ACTION( new App::Old::Action::Renderer::EnableSSAO( _preset, _enable ) );

		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			QT::QT_APP()->getMainWindow().updateRenderSetting( App::Old::Render::Renderer::RENDER_SETTING::SSAO );
		}
	};

	void EnableOutline::execute()
	{
		VTX_ACTION( new App::Old::Action::Renderer::EnableOutline( _preset, _enable ) );

		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			QT::QT_APP()->getMainWindow().updateRenderSetting( App::Old::Render::Renderer::RENDER_SETTING::OUTLINE );
		}
	};

	void EnableFog::execute()
	{
		VTX_ACTION( new App::Old::Action::Renderer::EnableFog( _preset, _enable ) );

		if ( App::Old::Application::RenderEffect::RenderEffectLibrary::get().isAppliedPreset( _preset ) )
		{
			QT::QT_APP()->getMainWindow().updateRenderSetting( App::Old::Render::Renderer::RENDER_SETTING::FOG );
		}
	};
} // namespace VTX::UI::Action::Renderer
