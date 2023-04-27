#ifndef __VTX_UI_ACTION_SETTING__
#define __VTX_UI_ACTION_SETTING__

#include "ui/core/base_main_window.hpp"
#include <app/core/action/base_action.hpp>
#include <app/old_app/renderer/base_renderer.hpp>
#include <util/logger.hpp>

namespace VTX::UI::Action::Setting
{
	class WindowMode : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit WindowMode( const Core::WindowMode & p_windowMode ) : _windowMode( p_windowMode ) {}
		void execute() override;

	  private:
		const Core::WindowMode _windowMode;
	};

	class ChangeShading : public VTX::App::Core::Action::BaseAction
	{
	  public:
		ChangeShading( const VTX::Renderer::SHADING p_shading ) : _shading( p_shading ) {}

		virtual void execute() override;

		virtual void displayUsage() override { VTX_INFO( "DIFFUSE|GLOSSY|TOON|FLAT_COLOR" ); }

	  private:
		const VTX::Renderer::SHADING _shading;
	};

	class ActiveAO : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit ActiveAO( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override;

	  private:
		const bool _active;
	};

	class ActiveOutline : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit ActiveOutline( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override;

	  private:
		const bool _active;
	};

	class ActiveFog : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit ActiveFog( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override;

	  private:
		const bool _active;
	};

	class ActiveAA : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit ActiveAA( const bool p_active ) : _active( p_active ) {}

		virtual void execute() override;

	  private:
		const bool _active;
	};

	class ApplyAllSettings : public VTX::App::Core::Action::BaseAction
	{
	  public:
		ApplyAllSettings( VTX::App::Application::Setting & p_setting ) : _setting( p_setting ) {}

		virtual void execute() override;

	  private:
		VTX::App::Application::Setting _setting;
	};
} // namespace VTX::UI::Action::Setting

#endif
