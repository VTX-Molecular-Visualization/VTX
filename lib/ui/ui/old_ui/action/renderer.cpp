#include "renderer.hpp"
#include "qt/application_qt.hpp"
#include "qt/main_window.hpp"
#include <old/action/action_manager.hpp>
#include <old/action/renderer.hpp>

namespace VTX::UI::Action::Renderer
{
	void ChangeShading::execute()
	{
		VTX_ACTION( new VTX::Action::Renderer::ChangeShading( _preset, _shading ) );

		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			QT::QT_APP()->getMainWindow().updateRenderSetting( VTX::Renderer::RENDER_SETTING::SHADING );
		}
	};

	void EnableSSAO::execute()
	{
		VTX_ACTION( new VTX::Action::Renderer::EnableSSAO( _preset, _enable ) );

		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			QT::QT_APP()->getMainWindow().updateRenderSetting( VTX::Renderer::RENDER_SETTING::SSAO );
		}
	};

	void EnableOutline::execute()
	{
		VTX_ACTION( new VTX::Action::Renderer::EnableOutline( _preset, _enable ) );

		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			QT::QT_APP()->getMainWindow().updateRenderSetting( VTX::Renderer::RENDER_SETTING::OUTLINE );
		}
	};

	void EnableFog::execute()
	{
		VTX_ACTION( new VTX::Action::Renderer::EnableFog( _preset, _enable ) );

		if ( Model::Renderer::RenderEffectPresetLibrary::get().isAppliedPreset( _preset ) )
		{
			QT::QT_APP()->getMainWindow().updateRenderSetting( VTX::Renderer::RENDER_SETTING::FOG );
		}
	};
} // namespace VTX::UI::Action::Renderer
