#include "base_model.hpp"
#include "mvc/mvc_manager.hpp"

namespace VTX
{
	namespace Model
	{
		void BaseModel::_notifyViews( const Event::VTXEvent * const p_event )
		{
			MVC::MvcManager::get().notifyViews( this, p_event );
			delete p_event;
		}
	} // namespace Model
} // namespace VTX
