#include "app/application/representation/primitive/sphere.hpp"
#include "app/old_app/setting.hpp"

namespace VTX::App::Application::Representation::Primitive
{
	Sphere::Sphere() :
		radiusFixed( Setting::ATOMS_RADIUS_DEFAULT ), radiusAdd( Setting::ATOMS_RADIUS_ADD_DEFAULT ),
		isRadiusFixed( Setting::ATOMS_IS_RADIUS_FIXED_DEFAULT )
	{
	}
} // namespace VTX::App::Application::Representation::Primitive
