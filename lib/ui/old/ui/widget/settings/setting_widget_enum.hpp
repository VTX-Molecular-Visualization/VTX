#ifndef __VTX_UI_WIDGET_SETTING_ENUM__
#define __VTX_UI_WIDGET_SETTING_ENUM__

#include <string>
#include <vector>

namespace VTX::UI::Widget::Settings
{
	enum class SETTING_MENU
	{
		REPRESENTATIONS = 0,
		RENDER_EFFECTS,
		MOLECULE,
		VTX,
		COLORS,

		COUNT
	};

	namespace VTXSettings
	{
		enum class CameraProjection : int
		{
			PERSPECTIVE,
			ORTHOGRAPHIC,

			COUNT
		};
		inline const static std::vector<std::string> CAMERA_PROJECTION_STR = { "Perspective", "Orthographic" };
	}; // namespace VTXSettings
} // namespace VTX::UI::Widget::Settings
#endif
