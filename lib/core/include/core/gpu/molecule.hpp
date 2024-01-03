#ifndef __VTX_CORE_GPU_MOLECULE__
#define __VTX_CORE_GPU_MOLECULE__

#include <util/color/rgba.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Core::Gpu
{
	struct Molecule
	{
		Mat4f						   transform;
		std::vector<Vec3f>			   atomPositions;
		std::vector<Util::Color::Rgba> atomColors;
		std::vector<float>			   atomRadii;
		std::vector<uchar>			   atomVisibilities;
		std::vector<uchar>			   atomSelections;
		std::vector<uint>			   atomIds;
		std::vector<uint>			   bonds;
	};
} // namespace VTX::Core::Gpu

#endif
