#ifndef __VTX_RENDERER_PROXY_MOLECULE__
#define __VTX_RENDERER_PROXY_MOLECULE__

#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer::Proxy
{
	struct Molecule
	{
		const Mat4f * transform;
		// const Mat4f * normals;
		const std::vector<Vec3f> *		  atomPositions;
		const std::vector<uint_fast8_t> * atomColors;
		const std::vector<float> *		  atomRadii;
		const std::vector<bool> *		  atomVisibilities; //
		const std::vector<bool> *		  atomSelections;	//
		const std::vector<uint> *		  atomIds;
		const std::vector<uint> *		  bonds;

		const std::vector<std::string> * atomNames; //
		const std::vector<uint> *		 residueIds;
		const std::vector<uchar> *		 residueSecondaryStructureTypes;
		const std::vector<uchar> *		 residueColors;
		const std::vector<uint> *		 residueFirstAtomIndexes; //
		const std::vector<uint> *		 residueAtomCounts;		  //
		const std::vector<size_t> *		 chainFirstResidues;	  //
		const std::vector<size_t> *		 chainResidueCounts;	  //
	};

} // namespace VTX::Renderer::Proxy

#endif
