#include "app/old/component/generic/base_auto_rotate.hpp"
#include "app/old/application/setting.hpp"

namespace VTX::App::Old::Component::Generic
{
	BaseAutoRotate::BaseAutoRotate() :
		_autoRotationVector( VTX::App::Old::Application::Setting::AUTO_ROTATE_SPEED_DEFAULT
							 * VTX::App::Old::Application::Setting::AUTO_ROTATE_ORIENTATION_DEFAULT )
	{
	}
} // namespace VTX::App::Old::Component::Generic
