#ifndef __VTX_APP_COMPONENT_RENDER_ENUM_CAMERA__
#define __VTX_APP_COMPONENT_RENDER_ENUM_CAMERA__

namespace VTX::App::Component::Render
{
	enum class CAMERA_PROJECTION : int
	{
		PERSPECTIVE,
		ORTHOGRAPHIC,

		COUNT
	};
} // namespace VTX::App::Component::Render
#endif
