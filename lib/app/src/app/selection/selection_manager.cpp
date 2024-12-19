#include "app/selection/selection_manager.hpp"

namespace VTX::App::Selection
{
	SelectionManager::SelectionManager() { _linkCallbackToSelectables(); };
	SelectionManager::~SelectionManager() = default;

	const Selection & SelectionManager::getCurrent() const { return *_currentSelection; }
	Selection &		  SelectionManager::getCurrent() { return *_currentSelection; }

	void SelectionManager::save( const std::string & p_name, std::unique_ptr<Selection> p_selection )
	{
		_savedSelectionMap[ p_name ] = std::move( p_selection );
	}
	void SelectionManager::erase( const std::string & p_name ) { _savedSelectionMap.erase( p_name ); }

	bool SelectionManager::hasSaved( const std::string & p_name ) const
	{
		return _savedSelectionMap.contains( p_name );
	}
	const Selection & SelectionManager::getSaved( const std::string & p_name ) const
	{
		return *_savedSelectionMap[ p_name ];
	}
	Selection & SelectionManager::getSaved( const std::string & p_name ) { return *_savedSelectionMap[ p_name ]; }

	void SelectionManager::apply( const Selection & p_selection ) { _setCurrentSelection( p_selection ); }
	void SelectionManager::apply( const std::string & p_selectionName )
	{
		_setCurrentSelection( *_savedSelectionMap[ p_selectionName ] );
	}

	void SelectionManager::_setCurrentSelection( const Selection & p_selection )
	{
		// Clear Callbacks ?
		_currentSelection = std::make_unique<Selection>( p_selection );
		_linkCallbackToSelectables();
	}

	void SelectionManager::_linkCallbackToSelectables()
	{
		_currentSelection->onSelect += []( const SelectionData & p_selectionData )
		{ p_selectionData.getSelectionComponent().onSelect( p_selectionData ); };

		_currentSelection->onDeselect += []( const SelectionData & p_selectionData )
		{ p_selectionData.getSelectionComponent().onDeselect( p_selectionData ); };
	}

} // namespace VTX::App::Selection
