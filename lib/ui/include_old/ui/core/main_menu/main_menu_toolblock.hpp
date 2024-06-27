#ifndef __VTX_UI_CORE_MAIN_MENU_MENU_TOOLBLOCK__
#define __VTX_UI_CORE_MAIN_MENU_MENU_TOOLBLOCK__

#include "main_menu_toolbutton.hpp"
#include "ui/core/container/grid_container.hpp"
#include <string>

namespace VTX::UI::Core::MainMenu
{
	class MainMenuTooltab;

	class MainMenuToolBlock
	{
	  protected:
		using ToolButtonContainer = Container::GridContainer<MainMenuToolButton>;

	  public:
		~MainMenuToolBlock() = default;

		virtual void			  setTitle( const std::string & p_title ) = 0;
		virtual const std::string getTitle() const						  = 0;

		const MainMenuTooltab * const getTooltab() const { return _linkedTooltab; }
		void						  setTooltab( const MainMenuTooltab * p_tooltab ) { _linkedTooltab = p_tooltab; }

		virtual void pushButton( MainMenuToolButton & p_toolButton );
		virtual void pushButton( MainMenuToolButton & p_toolButton1, MainMenuToolButton & p_toolButton2 );
		virtual void pushButton( MainMenuToolButton & p_toolButton1,
								 MainMenuToolButton & p_toolButton2,
								 MainMenuToolButton & p_toolButton3 );

		int getColumnCount() const;

		virtual void clear();

	  protected:
		MainMenuToolBlock() {};
		const ToolButtonContainer & _getToolButtons() const { return _toolButtons; };

	  private:
		ToolButtonContainer		_toolButtons   = ToolButtonContainer();
		const MainMenuTooltab * _linkedTooltab = nullptr;
	};
} // namespace VTX::UI::Core::MainMenu
#endif
