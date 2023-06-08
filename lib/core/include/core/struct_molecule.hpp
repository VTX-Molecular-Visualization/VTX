#ifndef __VTX_CORE_STRUCT_MOLECULE__
#define __VTX_CORE_STRUCT_MOLECULE__

#include <util/color/rgba.hpp>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Core
{
	struct StructMolecule
	{
		Mat4f						   tranform;
		std::vector<Vec3f>			   atomPositions;
		std::vector<Util::Color::Rgba> atomColors;
		std::vector<float>			   atomRadii;
		std::vector<uint>			   atomVisibilities;
		std::vector<uint>			   atomSelections;
		std::vector<uint>			   atomIds;
		std::vector<uint>			   bonds;
	};
} // namespace VTX::Core

#endif
