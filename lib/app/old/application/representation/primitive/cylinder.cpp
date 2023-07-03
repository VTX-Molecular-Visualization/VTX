#include "app/old/application/representation/primitive/cylinder.hpp"
#include "app/old/application/setting.hpp"

namespace VTX::App::Old::Application::Representation::Primitive
{
	Cylinder::Cylinder() :
		radius( VTX::App::Old::Application::Setting::BONDS_RADIUS_DEFAULT ), colorBlendingMode( VTX::App::Old::Application::Setting::BONDS_COLOR_BLENDING_MODE_DEFAULT )
	{
	}
} // namespace VTX::App::Old::Application::Representation::Primitive
