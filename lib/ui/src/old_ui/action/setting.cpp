#include "ui/old_ui/action/setting.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/main_window.hpp"
#include <app/old_app/action/action_manager.hpp>
#include <app/old_app/action/setting.hpp>

namespace VTX::UI::Action::Setting
{
	void ChangeShading::execute()
	{
		VTX_ACTION( new Action::Setting::ChangeShading( _shading ) );
		QT::QT_APP()->getMainWindow().updateRenderSetting( VTX::Renderer::RENDER_SETTING::SHADING );
	}

	void ActiveAO::execute()
	{
		VTX_ACTION( new Action::Setting::ActiveAO( _active ) );
		QT::QT_APP()->getMainWindow().updateRenderSetting( VTX::Renderer::RENDER_SETTING::SSAO );
	}

	void ActiveOutline::execute()
	{
		VTX_ACTION( new Action::Setting::ActiveOutline( _active ) );
		QT::QT_APP()->getMainWindow().updateRenderSetting( VTX::Renderer::RENDER_SETTING::OUTLINE );
	}

	void ActiveFog::execute()
	{
		VTX_ACTION( new Action::Setting::ActiveFog( _active ) );
		QT::QT_APP()->getMainWindow().updateRenderSetting( VTX::Renderer::RENDER_SETTING::FOG );
	}

	void ActiveAA::execute()
	{
		VTX_ACTION( new Action::Setting::ActiveAA( _active ) );
		QT::QT_APP()->getMainWindow().updateRenderSetting( VTX::Renderer::RENDER_SETTING::AA );
	}

	void WindowMode::execute()
	{
		VTX_SETTING().setWindowFullscreen( _windowMode == Core::WindowMode::Fullscreen );
		QT::QT_APP()->getMainWindow().setWindowMode( _windowMode );
	}

	void ApplyAllSettings::execute()
	{
		VTX_ACTION( new Action::Setting::ApplyAllSettings( _setting ) );

		VTX_ACTION( new Action::Setting::WindowMode( _setting.getWindowFullscreen() ? Core::WindowMode::Fullscreen
																					: Core::WindowMode::Windowed ) );
	}
} // namespace VTX::UI::Action::Setting
