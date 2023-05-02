#include "app/application/selection/selection_manager.hpp"
#include "app/application/selection/selection.hpp"
#include "app/event.hpp"
#include "app/event/global.hpp"
#include "app/mvc.hpp"
#include <string>
#include <util/logger.hpp>

namespace VTX::App::Application::Selection
{
	SelectionManager::SelectionManager()
	{
		_selectionModel = VTX::MVC_MANAGER().instantiateModel<App::Application::Selection::SelectionModel>();
		VTX_EVENT<App::Application::Selection::SelectionModel *>( VTX::App::Event::Global::SELECTION_ADDED,
																  _selectionModel );
	}
	SelectionManager::~SelectionManager() {}

	void SelectionManager::deleteModel() { VTX::MVC_MANAGER().deleteModel( _selectionModel ); }
} // namespace VTX::App::Application::Selection
