#ifndef __VTX_UI_CORE_MAIN_MENU_TOOLTAB__
#define __VTX_UI_CORE_MAIN_MENU_TOOLTAB__

#include "core/define.hpp"
#include <map>
#include <vector>

namespace VTX::UI::Core::MainMenu
{
	class MainMenuToolBlock;

	class MainMenuTooltab
	{
	  public:
		virtual MainMenuToolBlock & getToolBlock( const WidgetKey & p_key );

		const MainMenuToolBlock * const getPreviousToolBlock( const MainMenuToolBlock * const p_from,
															  const bool					  p_loop = true ) const;
		const MainMenuToolBlock * const getNextToolBlock( const MainMenuToolBlock * const p_from,
														  const bool					  p_loop = true ) const;

	  protected:
		virtual MainMenuToolBlock * _instantiateBlock() = 0;
		virtual void				_addBlock( const WidgetKey & p_tabID, MainMenuToolBlock * const p_block );
		virtual void				_addSeparator() = 0;

		const std::vector<MainMenuToolBlock *> & _getToolblocks() const { return _toolBlocks; }
		int										 _getToolblockCount() const;

	  private:
		std::vector<MainMenuToolBlock *>		 _toolBlocks	   = std::vector<MainMenuToolBlock *>();
		std::map<WidgetKey, MainMenuToolBlock *> _mapKeyToolBlocks = std::map<WidgetKey, MainMenuToolBlock *>();
	};
} // namespace VTX::UI::Core::MainMenu
#endif
