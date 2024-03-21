#ifndef __VTX_UI_CORE_BASE_UI_APPLICATION__
#define __VTX_UI_CORE_BASE_UI_APPLICATION__

#include <string>
#include <util/callback.hpp>
#include <vector>

namespace VTX::UI::Core
{
	class BaseUIApplication
	{
	  public:
		BaseUIApplication();

		virtual void init();
		virtual void start( const std::vector<std::string> & p_args );

		virtual void update();

		virtual void stop();
		virtual void quit();

		int getReturnCode() const { return _returnCode; };

		Util::Callback<> onInitEnded;

	  protected:
		int _returnCode = EXIT_SUCCESS;

		virtual void _initVTXApp( const std::vector<std::string> & p_args );
		virtual void _initUI( const std::vector<std::string> & p_args ) = 0;
		void		 _buildUI();
		virtual void _startUI( const std::vector<std::string> & p_args ) = 0;

		virtual void _postInit( const std::vector<std::string> & p_args );
	};

} // namespace VTX::UI::Core

#endif
