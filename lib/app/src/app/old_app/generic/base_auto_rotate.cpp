#include "app/old_app/generic/base_auto_rotate.hpp"
#include "app/application/setting.hpp"

namespace VTX::Generic
{
	BaseAutoRotate::BaseAutoRotate() :
		_autoRotationVector( VTX::App::Application::Setting::AUTO_ROTATE_SPEED_DEFAULT * VTX::App::Application::Setting::AUTO_ROTATE_ORIENTATION_DEFAULT )
	{
	}
} // namespace VTX::Generic
