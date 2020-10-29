#include "base_model.hpp"
#include "mvc/mvc_manager.hpp"

namespace VTX
{
	namespace Model
	{
		void BaseModel::_notifyViews( const Event::VTX_EVENT_MODEL & p_event ) { _notifyViews( p_event, nullptr ); }

		void BaseModel::_notifyViews( const Event::VTX_EVENT_MODEL & p_event, const Event::VTXEventModelData * const p_eventData )
		{
			MVC::MvcManager::get().notifyView( this, p_event, p_eventData );
		};
	} // namespace Model
} // namespace VTX
