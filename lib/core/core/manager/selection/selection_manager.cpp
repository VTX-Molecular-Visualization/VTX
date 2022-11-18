#include "selection_manager.hpp"
#include "manager/event/event_manager.hpp"
#include "manager/mvc/mvc_manager.hpp"
#include "model/selection/selection.hpp"
#include "tool/logger.hpp"
#include <string>

namespace VTX
{
	namespace Selection
	{
		SelectionManager::SelectionManager()
		{
			_selectionModel = MVC::MvcManager::get().instantiateModel<Model::Selection>();
			VTX_EVENT( new Event::VTXEventPtr( Event::Global::SELECTION_ADDED, _selectionModel ) );
		}
		SelectionManager::~SelectionManager() {}

		void SelectionManager::deleteModel() { MVC::MvcManager::get().deleteModel( _selectionModel ); }
	} // namespace Selection
} // namespace VTX
