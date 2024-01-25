#ifndef __VTX_RENDERER_PROXY_MOLECULE__
#define __VTX_RENDERER_PROXY_MOLECULE__

#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer::Proxy
{
	struct Molecule
	{
		Mat4f * transform;
		// const Mat4f * normals;
		std::vector<Vec3f> * atomPositions;
		std::vector<uint_fast8_t> * atomColors;
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
