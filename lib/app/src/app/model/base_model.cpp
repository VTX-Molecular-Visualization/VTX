#include "app/model/base_model.hpp"
#include "app/core/mvc/mvc_manager.hpp"

namespace VTX
{
	namespace Model
	{
		void BaseModel::_notifyViews( const Event::VTXEvent * const p_event )
		{
			VTX::Core::MVC::MvcManager::get().notifyViews( this, p_event );
			delete p_event;
		}
	} // namespace Model
} // namespace VTX
