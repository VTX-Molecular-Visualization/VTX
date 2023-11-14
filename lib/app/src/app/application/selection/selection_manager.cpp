#include "app/application/selection/selection_manager.hpp"

namespace VTX::App::Application::Selection
{
	SelectionManager::SelectionManager()  = default;
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

	void SelectionManager::apply( const Selection & p_selection )
	{
		_currentSelection = std::make_unique<Selection>( p_selection );
	}
	void SelectionManager::apply( const std::string & p_selectionName )
	{
		_currentSelection = std::make_unique<Selection>( *_savedSelectionMap[ p_selectionName ] );
	}

} // namespace VTX::App::Application::Selection
