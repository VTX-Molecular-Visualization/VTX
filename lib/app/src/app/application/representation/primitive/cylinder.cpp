#include "app/application/representation/primitive/cylinder.hpp"
#include "app/application/setting.hpp"

namespace VTX::App::Application::Representation::Primitive
{
	Cylinder::Cylinder() :
		radius( VTX::App::Application::Setting::BONDS_RADIUS_DEFAULT ), colorBlendingMode( VTX::App::Application::Setting::BONDS_COLOR_BLENDING_MODE_DEFAULT )
	{
	}
} // namespace VTX::App::Application::Representation::Primitive
