#include "selection_manager.hpp"
#include "event/event_manager.hpp"
#include "mvc/mvc_manager.hpp"
#include "tool/logger.hpp"
#include <string>

namespace VTX
{
	namespace Selection
	{
		SelectionManager::SelectionManager() { _selectionModel = MVC::MvcManager::get().instantiate<Model::Selection>(); }

		SelectionManager::~SelectionManager() { MVC::MvcManager::get().deleteModel( _selectionModel ); }

	} // namespace Selection
} // namespace VTX
