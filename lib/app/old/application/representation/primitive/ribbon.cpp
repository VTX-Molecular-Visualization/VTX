#include "app/old/application/representation/primitive/ribbon.hpp"
#include "app/old/application/setting.hpp"

namespace VTX::App::Old::Application::Representation::Primitive
{
	Ribbon::Ribbon() :
		colorMode( VTX::App::Old::Application::Setting::SS_COLOR_MODE_DEFAULT ), colorBlendingMode( VTX::App::Old::Application::Setting::SS_COLOR_BLENDING_MODE_DEFAULT )
	{
	}

} // namespace VTX::App::Old::Application::Representation::Primitive
