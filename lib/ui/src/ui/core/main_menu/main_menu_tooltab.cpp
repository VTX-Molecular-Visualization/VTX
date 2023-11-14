#include "ui/core/main_menu/main_menu_tooltab.hpp"
#include "ui/core/main_menu/main_menu_toolblock.hpp"

namespace VTX::UI::Core::MainMenu
{
	MainMenuToolBlock & MainMenuTooltab::getToolBlock( const WidgetKey & p_key )
	{
		std::map<WidgetKey, MainMenuToolBlock *>::const_iterator it = _mapKeyToolBlocks.find( p_key );

		MainMenuToolBlock * res;

		if ( it != _mapKeyToolBlocks.end() )
		{
			res = it->second;
		}
		else
		{
			if ( _toolBlocks.size() > 0 )
				_addSeparator();

			res = _instantiateBlock();
			res->setTooltab( this );
			res->setTitle( p_key );
			_addBlock( p_key, res );
		}

		return *res;
	}

	void MainMenuTooltab::_addBlock( const WidgetKey & p_blockID, MainMenuToolBlock * const p_block )
	{
		_toolBlocks.emplace_back( p_block );
		_mapKeyToolBlocks[ p_blockID ] = p_block;
	}

	int MainMenuTooltab::_getToolblockCount() const { return int( _toolBlocks.size() ); }

	const MainMenuToolBlock * const MainMenuTooltab::getPreviousToolBlock( const MainMenuToolBlock * const p_from,
																		   const bool p_loop ) const
	{
		const MainMenuToolBlock * previous = p_loop ? _toolBlocks[ _toolBlocks.size() - 1 ] : nullptr;

		for ( int i = 0; i < _toolBlocks.size(); i++ )
		{
			if ( _toolBlocks[ i ] == p_from )
				break;

			previous = _toolBlocks[ i ];
		}

		return previous;
	}
	const MainMenuToolBlock * const MainMenuTooltab::getNextToolBlock( const MainMenuToolBlock * const p_from,
																	   const bool					   p_loop ) const
	{
		const MainMenuToolBlock * next = p_loop ? _toolBlocks[ 0 ] : nullptr;

		for ( size_t i = _toolBlocks.size() - 1; i >= 0; i-- )
		{
			if ( _toolBlocks[ i ] == p_from )
				break;

			next = _toolBlocks[ i ];
		}

		return next;
	}

} // namespace VTX::UI::Core::MainMenu
