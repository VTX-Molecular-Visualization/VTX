#include "base_auto_rotate.hpp"
#include "old_app/setting.hpp"

namespace VTX::Generic
{
	BaseAutoRotate::BaseAutoRotate() :
		_autoRotationVector( Setting::AUTO_ROTATE_SPEED_DEFAULT * Setting::AUTO_ROTATE_ORIENTATION_DEFAULT )
	{
	}
} // namespace VTX::Generic
