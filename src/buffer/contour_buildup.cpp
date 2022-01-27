#include "contour_buildup.hpp"

namespace VTX::Buffer
{
	void ContourBuildup::generate()
	{
		_ssboTorusPatches.create();
		_ssboSphericalTrianglePatches.create();
		_ssboProbeIntersection.create();

		_vao.create();
	}
} // namespace VTX::Buffer
