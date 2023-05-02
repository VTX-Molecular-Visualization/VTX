#include "app/application/representation/primitive/ribbon.hpp"
#include "app/application/setting.hpp"

namespace VTX::App::Application::Representation::Primitive
{
	Ribbon::Ribbon() :
		colorMode( VTX::App::Application::Setting::SS_COLOR_MODE_DEFAULT ), colorBlendingMode( VTX::App::Application::Setting::SS_COLOR_BLENDING_MODE_DEFAULT )
	{
	}

} // namespace VTX::App::Application::Representation::Primitive
