#include "app/old/application/representation/primitive/sphere.hpp"
#include "app/old/application/setting.hpp"

namespace VTX::App::Old::Application::Representation::Primitive
{
	Sphere::Sphere() :
		radiusFixed( VTX::App::Old::Application::Setting::ATOMS_RADIUS_DEFAULT ), radiusAdd( VTX::App::Old::Application::Setting::ATOMS_RADIUS_ADD_DEFAULT ),
		isRadiusFixed( VTX::App::Old::Application::Setting::ATOMS_IS_RADIUS_FIXED_DEFAULT )
	{
	}
} // namespace VTX::App::Old::Application::Representation::Primitive
