#include "app/old_app/selection/selection_manager.hpp"
#include "app/old_app/event/event_manager.hpp"
#include "app/old_app/model/selection.hpp"
#include "app/old_app/mvc/mvc_manager.hpp"
#include <string>
#include <util/logger.hpp>

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
