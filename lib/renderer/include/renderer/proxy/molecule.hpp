#ifndef __VTX_RENDERER_PROXY_MOLECULE__
#define __VTX_RENDERER_PROXY_MOLECULE__

#include <util/callback.hpp>
#include <util/math/range_list.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Renderer::Proxy
{
	struct Molecule
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

		std::vector<uchar> & atomColors;
		std::vector<float> & atomRadii;
		std::vector<uint> &	 atomIds;
		std::vector<uchar> & residueColors;
		std::vector<uint> &	 residueIds;

		const uint idDefaultRepresentation = 0;

		Util::Callback<>					 onTransform;	   // Move/rotate.
		Util::Callback<bool>				 onVisible;		   // Show/hide the whole molecule.
		Util::Callback<bool>				 onSelect;		   // Select/deselect the whole molecule.
		Util::Callback<uint>				 onRepresentation; // Change the representation of the whole molecule.
		Util::Callback<>					 onRemove;		   // Remove proxy.
		Util::Callback<>					 onAtomPositions;  // Update dynamics.
		Util::Callback<std::vector<uchar> &> onAtomColors;	   // Update colors (ATOM/CHAIN/RESIDUE)...
		Util::Callback<std::vector<uchar> &> onResidueColors;  // Update colors (ATOM/CHAIN/RESIDUE)...
		Util::Callback<Util::Math::RangeList<uint> &, bool> onAtomVisibilities; // Update atom visibility with ranges.
		Util::Callback<Util::Math::RangeList<uint> &, bool> onAtomSelections;	// Update atom selection with ranges.
		Util::Callback<Util::Math::RangeList<uint> &, uint>
			onAtomRepresentations; // Update atom representations with ranges.
	};

} // namespace VTX::Renderer::Proxy

#endif
