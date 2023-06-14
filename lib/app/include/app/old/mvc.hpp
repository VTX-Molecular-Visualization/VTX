#ifndef __VTX_APP_OLD_MVC__
#define __VTX_APP_OLD_MVC__

#include "manager/mvc_manager.hpp"

namespace VTX
{
	inline App::Old::Manager::MvcManager & MVC_MANAGER() { return App::Old::Manager::MvcManager::get(); }
} // namespace VTX

#endif
