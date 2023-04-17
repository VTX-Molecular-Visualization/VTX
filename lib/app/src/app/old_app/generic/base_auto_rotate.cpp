#include "app/old_app/generic/base_auto_rotate.hpp"
#include "app/old_app/setting.hpp"

namespace VTX::Generic
{
	BaseAutoRotate::BaseAutoRotate() :
		_autoRotationVector( Setting::AUTO_ROTATE_SPEED_DEFAULT * Setting::AUTO_ROTATE_ORIENTATION_DEFAULT )
	{
	}
} // namespace VTX::Generic
