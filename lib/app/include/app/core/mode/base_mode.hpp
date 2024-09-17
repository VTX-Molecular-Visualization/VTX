#ifndef __VTX_APP_CORE_MODE_BASE_MODE__
#define __VTX_APP_CORE_MODE_BASE_MODE__

#include "app/application/scene.hpp"
#include "app/core/controller/base_controller_camera.hpp"
#include "app/core/controller/concepts.hpp"
#include "app/vtx_app.hpp"
#include <util/collection.hpp>

namespace VTX::App::Core::Mode
{
	class BaseMode
	{
	  public:
		virtual ~BaseMode() = default;

		virtual void enter() = 0;
		virtual void exit()	 = 0;
	};

} // namespace VTX::App::Core::Mode

#endif
