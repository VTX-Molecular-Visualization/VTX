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

		const std::vector<Vec3f> * atomPositions; // Needed.
		// TODO: const std::vector<Vec3f> * atomPositionsNextFrame;  // To smooth dynamics.
		const std::vector<uchar> * atomColors; // Needed?
		const std::vector<float> * atomRadii;  // Not needed?
		// TODO: remove and cache?
		const std::vector<bool> * atomVisibilities; // Needed?
		const std::vector<bool> * atomSelections;	// Needed?
		// const std::vector<uchar> * onAtomRepresentations; // Needed?

		const std::vector<uint> * atomIds; // Not needed?
		const std::vector<uint> * bonds;   // Needed.

		const std::vector<std::string> * atomNames;						 // Needed to recompute ss.
		const std::vector<uint> *		 residueIds;					 // Needed to recompute ss.
		const std::vector<uchar> *		 residueSecondaryStructureTypes; // Needed to recompute ss.
		const std::vector<uchar> *		 residueColors;					 // Needed to recompute ss.
		const std::vector<uint> *		 residueFirstAtomIndexes;		 // Needed to recompute ss.
		const std::vector<uint> *		 residueAtomCounts;				 // Needed to recompute ss.
		const std::vector<size_t> *		 chainFirstResidues;			 // Needed to recompute ss.
		const std::vector<size_t> *		 chainResidueCounts;			 // Needed to recompute ss.

		// Default color layout and representation.
		const uchar idColorLayout	 = 0; // TODO.
		const uchar idRepresentation = 0;
		// // TODO: const bool smoothMD = false;

		Util::Callback<>	  onTransform;		// Move/rotate.
		Util::Callback<bool>  onSelect;			// Select/deselect the whole molecule.
		Util::Callback<bool>  onVisible;		// Show/hide the whole molecule.
		Util::Callback<uchar> onRepresentation; // Change the representation of the whole molecule.
		Util::Callback<>	  onRemove;			// Remove proxy.

		Util::Callback<> onAtomPositions; // Update dynamics.
		// Update dynamics: mix(atomPositions, atomPositionsNextFrame, t).
		Util::Callback<>	  onAtomPositionsNextFrame;
		Util::Callback<float> onUpate;
		// Use pointer, precomputed data passed to callback or compute in renderer?
		Util::Callback<>	  onAtomColors;				  // Update colors (ATOM/CHAIN/RESIDUE)...
		Util::Callback<>	  onAtomVisibilities;		  // Update visibilities.
		Util::Callback<>	  onAtomSelections;			  // Update selections.
		Util::Callback<uchar> onAtomRepresentations;	  // To discuss.
		Util::Callback<>	  onUpdateSecondaryStructure; // Recompute secondary structure.

		Util::Callback<uchar> onColorLayout; // TODO: Change the model color layout.

		// TODO:
		// onVisible()
	};

} // namespace VTX::Renderer::Proxy

#endif
