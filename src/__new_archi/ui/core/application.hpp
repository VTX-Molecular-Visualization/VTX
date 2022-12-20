#ifndef __VTX_UI_CORE_APPLICATION__
#define __VTX_UI_CORE_APPLICATION__

#include <string>
#include <vector>

namespace VTX::UI::Core
{
	class Application
	{
	  public:
		virtual void init();
		virtual void start( const std::vector<std::string> & p_args );

		virtual void update();

		virtual void stop();

		virtual void quit();

		int getReturnCode() const { return _returnCode; };

	  protected:
		int _returnCode = EXIT_SUCCESS;

		virtual void _initVTXApp( const std::vector<std::string> & p_args );
		virtual void _initUI( const std::vector<std::string> & p_args ) = 0;
		void		 _buildUI();

		virtual void _postInit( const std::vector<std::string> & p_args );

		virtual void _handleArgs( const std::vector<std::string> & p_args );
	};

} // namespace VTX::UI::Core

#endif
