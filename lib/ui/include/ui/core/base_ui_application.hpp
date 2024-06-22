#ifndef __VTX_UI_CORE_BASE_UI_APPLICATION__
#define __VTX_UI_CORE_BASE_UI_APPLICATION__

#include <app/vtx_app.hpp>
#include <string>
#include <util/callback.hpp>
#include <vector>

namespace VTX::UI::Core
{
	class BaseUIApplication : public App::VTXApp
	{
	  public:
		BaseUIApplication();

		virtual void start( const std::vector<std::string> & p_args );
		virtual void stop() {}

	  protected:
		void _buildUI();

		virtual void _initUI( const std::vector<std::string> & p_args )	 = 0;
		virtual void _startUI( const std::vector<std::string> & p_args ) = 0;
	};

} // namespace VTX::UI::Core

#endif
