#include "ui/core/main_menu/main_menu_toolblock.hpp"

namespace VTX::UI::Core::MainMenu
{
	void MainMenuToolBlock::pushButton( MainMenuToolButton & p_toolButton )
	{
		_toolButtons.appendItemInNewColumn( &p_toolButton );
	}
	void MainMenuToolBlock::pushButton( MainMenuToolButton & p_toolButton1, MainMenuToolButton & p_toolButton2 )
	{
		_toolButtons.appendItemInNewColumn( &p_toolButton1 );
		_toolButtons.appendItem( &p_toolButton2 );
	}
	void MainMenuToolBlock::pushButton( MainMenuToolButton & p_toolButton1,
										MainMenuToolButton & p_toolButton2,
										MainMenuToolButton & p_toolButton3 )
	{
		_toolButtons.appendItemInNewColumn( &p_toolButton1 );
		_toolButtons.appendItem( &p_toolButton2 );
		_toolButtons.appendItem( &p_toolButton3 );
	}

	void MainMenuToolBlock::clear() { _toolButtons.clear(); }

	int MainMenuToolBlock::getColumnCount() const { return _toolButtons.getColumnCount(); }

} // namespace VTX::UI::Core::MainMenu
