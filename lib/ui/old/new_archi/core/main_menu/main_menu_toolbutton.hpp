#ifndef __VTX_UI_CORE_MAIN_MENU_TOOLBUTTON__
#define __VTX_UI_CORE_MAIN_MENU_TOOLBUTTON__

namespace VTX::UI::Core::MainMenu
{
	class MainMenuToolButton
	{
	  public:
		MainMenuToolButton();
		virtual void showActiveFeedback( const bool p_activate );

	  private:
		bool _hasActiveFeedback = false;
	};
} // namespace VTX::UI::Core::MainMenu
#endif
