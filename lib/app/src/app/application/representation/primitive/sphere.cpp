#include "app/application/representation/primitive/sphere.hpp"
#include "app/application/setting.hpp"

namespace VTX::App::Application::Representation::Primitive
{
	Sphere::Sphere() :
		radiusFixed( VTX::App::Application::Setting::ATOMS_RADIUS_DEFAULT ), radiusAdd( VTX::App::Application::Setting::ATOMS_RADIUS_ADD_DEFAULT ),
		isRadiusFixed( VTX::App::Application::Setting::ATOMS_IS_RADIUS_FIXED_DEFAULT )
	{
	}
} // namespace VTX::App::Application::Representation::Primitive
