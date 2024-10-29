#ifndef __VTX_RENDERER_PROXY_SYSTEM__
#define __VTX_RENDERER_PROXY_SYSTEM__

#include <util/callback.hpp>
#include <util/math/range_list.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer::Proxy
{
	/**
	 * @brief System data provider.
	 */
	struct System
	{
		const Mat4f *					 transform;
		const std::vector<Vec3f> *		 atomPositions;
		const std::vector<uint> *		 bonds;
		const std::vector<std::string> * atomNames;
		const std::vector<uchar> *		 residueSecondaryStructureTypes;
		const std::vector<uint> *		 residueFirstAtomIndexes;
		const std::vector<uint> *		 residueAtomCounts;
		const std::vector<size_t> *		 chainFirstResidues;
		const std::vector<size_t> *		 chainResidueCounts;

		const std::vector<uchar>  atomColors;
		const std::vector<float>  atomRadii;
		const std::vector<uint>	  atomIds;
		const std::vector<uchar>  residueColors;
		const std::vector<uint>	  residueIds;
		const std::vector<size_t> categoryPolymerChainIds;
		const std::vector<size_t> categoryCarbohydrateChainIds;

		const uint idDefaultRepresentation = 0;

		Util::Callback<>					 onTransform;	   // Move/rotate.
		Util::Callback<bool>				 onVisible;		   // Show/hide the whole molecule.
		Util::Callback<bool>				 onSelect;		   // Select/deselect the whole molecule.
		Util::Callback<uchar>				 onRepresentation; // Change the representation of the whole molecule.
		Util::Callback<>					 onRemove;		   // Remove proxy.
		Util::Callback<>					 onAtomPositions;  // Update dynamics.
		Util::Callback<std::vector<uchar> &> onAtomColors;	   // Update colors (ATOM/CHAIN/RESIDUE)...
		Util::Callback<std::vector<uchar> &> onResidueColors;  // Update colors (ATOM/CHAIN/RESIDUE)...

		Util::Callback<const Util::Math::RangeList<uint> &, bool>
			onAtomVisibilities; // Update atom visibility with ranges.
		Util::Callback<const Util::Math::RangeList<uint> &, bool>
			onAtomSelections; // Update atom selection with ranges.
		Util::Callback<const Util::Math::RangeList<uint> &, uchar>
			onAtomRepresentations; // Update atom representations with ranges.
		Util::Callback<const Util::Math::RangeList<uint> &, uchar> onAtomColorsRange;
		Util::Callback<const Util::Math::RangeList<uint> &, uchar> onResidueColorsRange;
	};

} // namespace VTX::Renderer::Proxy

#endif
