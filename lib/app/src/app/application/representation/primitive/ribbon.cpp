#include "app/application/representation/primitive/ribbon.hpp"
#include "app/old_app/setting.hpp"

namespace VTX::App::Application::Representation::Primitive
{
	Ribbon::Ribbon() :
		colorMode( Setting::SS_COLOR_MODE_DEFAULT ), colorBlendingMode( Setting::SS_COLOR_BLENDING_MODE_DEFAULT )
	{
	}

} // namespace VTX::App::Application::Representation::Primitive
