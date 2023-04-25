#include "app/old_app/selection/selection_manager.hpp"
#include "app/mvc.hpp"
#include "app/event.hpp"
#include "app/event/global.hpp"
#include "app/model/selection.hpp"
#include <string>
#include <util/logger.hpp>

namespace VTX
{
	namespace Selection
	{
		SelectionManager::SelectionManager()
		{
			_selectionModel = VTX::MVC_MANAGER().instantiateModel<Model::Selection>();
			VTX_EVENT<Model::Selection *>( VTX::App::Event::Global::SELECTION_ADDED, _selectionModel );
		}
		SelectionManager::~SelectionManager() {}

		void SelectionManager::deleteModel() { VTX::MVC_MANAGER().deleteModel( _selectionModel ); }
	} // namespace Selection
} // namespace VTX
