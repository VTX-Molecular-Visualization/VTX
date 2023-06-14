#include "app/old/core/model/base_model.hpp"
#include "app/old/mvc.hpp"

namespace VTX::App::Old::Core::Model
{
	void BaseModel::_notifyViews( const App::Old::Core::Event::VTXEvent * const p_event )
	{
		VTX::MVC_MANAGER().notifyViews( this, p_event );
		delete p_event;
	}
} // namespace VTX::App::Old::Core::Model
