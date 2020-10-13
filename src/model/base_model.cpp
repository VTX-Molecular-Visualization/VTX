#include "base_model.hpp"
#include "mvc/mvc_manager.hpp"

namespace VTX
{
	namespace Model
	{
		void BaseModel::_notifyViews( const Event::VTX_EVENT_MODEL & p_event ) { MVC::MvcManager::get().notifyView( this, p_event ); };
	} // namespace Model
} // namespace VTX
