#ifndef __VTX_RENDERER_PROXY_MOLECULE__
#define __VTX_RENDERER_PROXY_MOLECULE__

#include <util/color/rgba.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer::Proxy
{
	struct Molecule
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
		std::vector<size_t> *	   chainFirstResidues;		//
		std::vector<size_t> *	   chainResidueCounts;		//
	};

} // namespace VTX::Renderer::Proxy

#endif
