#ifndef __VTX_STYLE__
#define __VTX_STYLE__

namespace VTX
{
	namespace Style
	{
		enum class SYMBOL_DISPLAY_MODE : int
		{
			SHORT,
			LONG,

			COUNT
		};

		inline static const std::string VIEWPOINT_DEFAULT_NAME = "Viewpoint";

		inline static const int WINDOW_WIDTH_DEFAULT  = 1280;
		inline static const int WINDOW_HEIGHT_DEFAULT = 720;

	} // namespace Style
} // namespace VTX

#endif
