#ifndef __VTX_RENDERER_PROXY_MOLECULE__
#define __VTX_RENDERER_PROXY_MOLECULE__

#include <util/callback.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer::Proxy
{
	struct Molecule
	{
		const Mat4f * transform;

		const std::vector<Vec3f> * atomPositions;
		const std::vector<uchar> * atomColors;
		const std::vector<float> * atomRadii;
		const std::vector<bool> *  atomVisibilities; //
		const std::vector<bool> *  atomSelections;	 //
		const std::vector<uint> *  atomIds;
		const std::vector<uint> *  bonds;

		const std::vector<std::string> * atomNames; //
		const std::vector<uint> *		 residueIds;
		const std::vector<uchar> *		 residueSecondaryStructureTypes;
		const std::vector<uchar> *		 residueColors;
		const std::vector<uint> *		 residueFirstAtomIndexes; //
		const std::vector<uint> *		 residueAtomCounts;		  //
		const std::vector<size_t> *		 chainFirstResidues;	  //
		const std::vector<size_t> *		 chainResidueCounts;	  //

		Util::Callback<> onTransform;
	};

} // namespace VTX::Renderer::Proxy

#endif
