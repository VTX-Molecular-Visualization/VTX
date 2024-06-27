#ifndef __VTX_UI_CORE_BASE_UI_APPLICATION__
#define __VTX_UI_CORE_BASE_UI_APPLICATION__

#include <app/vtx_app.hpp>
#include <string>
#include <ui/core/layout_descriptor.hpp>
#include <util/callback.hpp>
#include <vector>

namespace VTX::UI::Core
{
	class BaseUIApplication : public App::VTXApp
	{
	  public:
		BaseUIApplication();

		void start( const std::vector<std::string> & p_args ) override;
		void stop() override;

		// TODO: addButton/addPanel

	  protected:
		// TODO: concept?
		virtual void _init( const std::vector<std::string> & p_args ) = 0;
		// virtual void _createMainWindow()							   = 0;
		virtual void _build( const LayoutDescriptor & )				   = 0;
		virtual void _start( const std::vector<std::string> & p_args ) = 0;
		virtual void _stop()										   = 0;
	};

} // namespace VTX::UI::Core

#endif
