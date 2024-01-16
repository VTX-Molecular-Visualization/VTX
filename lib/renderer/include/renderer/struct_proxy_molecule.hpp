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
		std::vector<bool> *	 atomVisibilities; //
		std::vector<bool> *	 atomSelections;   //
		std::vector<uint> *	 atomIds;
		std::vector<uint> *	 bonds;

		std::vector<std::string> * atomNames; //
		std::vector<uint> *		   residueIds;
		std::vector<uchar> *	   residueSecondaryStructureTypes;
		std::vector<uchar> *	   residueColors;
		std::vector<uint> *		   residueFirstAtomIndexes; //
		std::vector<uint> *		   residueAtomCounts;		//
		std::vector<uint> *		   chainFirstResidues;		//
		std::vector<uint> *		   chainResidueCounts;		//
	};

} // namespace VTX::Renderer

#endif
