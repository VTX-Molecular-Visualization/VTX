#ifndef __VTX_APP_MVC__
#define __VTX_APP_MVC__

#include "manager/mvc_manager.hpp"

namespace VTX
{
	inline App::Manager::MvcManager & MVC_MANAGER() { return App::Manager::MvcManager::get(); }
} // namespace VTX

#endif
