#ifndef __VTX_APP_OLD_COMPONENT_VIDEO_ENUM_PATH__
#define __VTX_APP_OLD_COMPONENT_VIDEO_ENUM_PATH__

namespace VTX::App::Old::Component::Video
{
	enum class PATH_DURATION_MODE : int
	{
		CONSTANT_SPEED,
		PATH,
		VIEWPOINT
	};

	enum class PATH_INTERPOLATION_MODE : int
	{
		LINEAR,
		CATMULL_ROM,
		CUBIC
	};

} // namespace VTX::App::Old::Component::Video
#endif
