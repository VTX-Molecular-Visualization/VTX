#include "app/application/representation/primitive/cylinder.hpp"
#include "app/old_app/setting.hpp"

namespace VTX::App::Application::Representation::Primitive
{
	Cylinder::Cylinder() :
		radius( Setting::BONDS_RADIUS_DEFAULT ), colorBlendingMode( Setting::BONDS_COLOR_BLENDING_MODE_DEFAULT )
	{
	}
} // namespace VTX::App::Application::Representation::Primitive
