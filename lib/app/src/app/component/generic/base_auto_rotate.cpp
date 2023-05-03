#include "app/component/generic/base_auto_rotate.hpp"
#include "app/application/setting.hpp"

namespace VTX::App::Component::Generic
{
	BaseAutoRotate::BaseAutoRotate() :
		_autoRotationVector( VTX::App::Application::Setting::AUTO_ROTATE_SPEED_DEFAULT
							 * VTX::App::Application::Setting::AUTO_ROTATE_ORIENTATION_DEFAULT )
	{
	}
} // namespace VTX::App::Component::Generic
