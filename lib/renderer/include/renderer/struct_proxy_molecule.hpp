#ifndef __VTX_RENDERER_STRUCT_PROXY_MOLECULE__
#define __VTX_RENDERER_STRUCT_PROXY_MOLECULE__

#include <util/color/rgba.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer
{
	struct StructProxyMolecule
	{
		const Mat4f * transform;
		// const Mat4f * normals;
		std::vector<Vec3f> * atomPositions;
		std::vector<uchar> * atomColors;
		std::vector<float> * atomRadii;
		std::vector<bool> *	 atomVisibilities;
		std::vector<bool> *	 atomSelections;
		std::vector<uint> *	 atomIds;
		std::vector<uint> *	 bonds;
		std::vector<uchar> * secondaryStructures;
	};

} // namespace VTX::Renderer

#endif
