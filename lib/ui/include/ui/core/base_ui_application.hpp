#ifndef __VTX_UI_CORE_BASE_UI_APPLICATION__
#define __VTX_UI_CORE_BASE_UI_APPLICATION__

#include <app/vtx_app.hpp>
#include <string>
#include <util/callback.hpp>
#include <vector>

namespace VTX::UI::Core
{
	struct LayoutDescriptor
	{
		struct ButtonDescriptor
		{
			std::string		 name;
			Util::Callback<> callback;
		};
	};

	class BaseUIApplication : public App::VTXApp
	{
	  public:
		BaseUIApplication();

		void start( const std::vector<std::string> & p_args ) override;
		void stop() override;

	  protected:
		// TODO: concept?
		virtual void _init( const std::vector<std::string> & p_args )  = 0;
		virtual void _build( const LayoutDescriptor & )				   = 0;
		virtual void _start( const std::vector<std::string> & p_args ) = 0;
	};

} // namespace VTX::UI::Core

#endif
