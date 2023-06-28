#include "app/old/application/selection/selection_manager.hpp"
#include "app/old/application/selection/selection.hpp"
#include "app/old/event.hpp"
#include "app/old/event/global.hpp"
#include "app/old/mvc.hpp"
#include <string>
#include <util/logger.hpp>

namespace VTX::App::Old::Application::Selection
{
	SelectionManager::SelectionManager()
	{
		_selectionModel = VTX::MVC_MANAGER().instantiateModel<App::Old::Application::Selection::SelectionModel>();
		VTX_EVENT<App::Old::Application::Selection::SelectionModel *>( VTX::App::Old::Event::Global::SELECTION_ADDED,
																  _selectionModel );
	}
	SelectionManager::~SelectionManager() {}

	void SelectionManager::deleteModel() { VTX::MVC_MANAGER().deleteModel( _selectionModel ); }
} // namespace VTX::App::Old::Application::Selection
