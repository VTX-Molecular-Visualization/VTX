#include "bspline.hpp"

namespace VTX
{
	namespace Math
	{
		const Mat4f BSpline::BSPLINE_MATRIX = { { -1.f / 6.f, 1.f / 2.0f, -1.f / 2.f, 1.f / 6.f },
												{ 1.f / 2.f, -1.f, 1.f / 2.f, 0.f },
												{ -1.f / 2.f, 0.f, 1.f / 2.f, 0.f },
												{ 1.f / 6.f, 2.f / 3.f, 1.f / 6.f, 0.f } };

	} // namespace Math
} // namespace VTX
