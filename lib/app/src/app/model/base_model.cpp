#include "app/core/model/base_model.hpp"
#include "app/mvc.hpp"

namespace VTX::App::Core::Model
{
	void BaseModel::_notifyViews( const App::Core::Event::VTXEvent * const p_event )
	{
		VTX::MVC_MANAGER().notifyViews( this, p_event );
		delete p_event;
	}
} // namespace VTX::App::Core::Model
