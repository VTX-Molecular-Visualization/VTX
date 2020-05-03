#include "color.hpp"

namespace VTX
{
	namespace Util
	{
		const Vec3f Color::black( 0.f, 0.f, 0.f );
		const Vec3f Color::white( 1.f, 1.f, 1.f );
		const Vec3f Color::red( 1.f, 0.f, 0.f );
		const Vec3f Color::green( 0.f, 1.f, 0.f );
		const Vec3f Color::blue( 0.f, 0.f, 1.f );
		const Vec3f Color::yellow( 1.f, 1.f, 0.f );
		const Vec3f Color::magenta( 1.f, 0.f, 1.f );
		const Vec3f Color::cyan( 0.f, 1.f, 1.f );

		// TODO: only used in CPU ray tracing mode for the moment...
		const std::vector<Vec3f> Color::predefinedChainColors = {
			// named with https://www.color-blindness.com/color-name-hue/
			{ 0.145f, 0.886f, 0.906f }, // turquoise
			{ 1.f, 0.247f, 0.4f },		// radical red
			{ 0.969f, 0.772f, 0.172f }, // saffron (yellow)
			{ 0.659f, 0.114f, 0.686f }, // dark orchid (violet)
			{ 0.129f, 0.768f, 0.204f }, // lime green
			{ 0.325f, 0.549f, 0.851f }, // picton blue (tinker-hp ^^)
			{ 0.894f, 0.204f, 0.216f }, // cinnabar (xlim red ^^)
			{ 0.663f, 0.572f, 0.407f }, // sandal (lct brown ^^)
			{ 0.804f, 0.f, 0.878f },	// psychedelic purple
			{ 0.129f, 0.918f, 0.071f }, // free speech green
			{ 0.145f, 0.274f, 0.856f }, // cerulean blue
			{ 0.787f, 0.071f, 0.176f }, // fire engine red (cnam ^^)
			{ 0.929f, 0.976f, 0.f },	// chartreuse yellow
			{ 0.878f, 0.262f, 0.835f }, // free speech magenta
			{ 0.682f, 0.902f, 0.153f }, // inch worm (green)
			{ 0.918f, 0.541f, 0.016f }, // tangerine (orange)
		};

	} // namespace Util
} // namespace VTX
