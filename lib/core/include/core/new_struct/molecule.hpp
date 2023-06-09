#ifndef __VTX_CORE_STRUCT_MOLECULE_DATA__
#define __VTX_CORE_STRUCT_MOLECULE_DATA__

#include "_fwd.hpp"
#include "core/chemdb/category.hpp"
#include "core/gpu/molecule.hpp"
#include "core/new_struct/trajectory.hpp"
#include <array>
#include <util/color/rgba.hpp>
#include <vector>

namespace VTX::Core::Struct
{
	class Molecule
	{
	  private:
		static const int CATEGORY_COUNT = int( ChemDB::Category::TYPE::COUNT );

	  public:
		// GPU
		Gpu::Molecule gpuStruct = Gpu::Molecule();

		// Logic
		std::string name = "unknown";

		std::array<Category *, CATEGORY_COUNT> categories				  = std::array<Category *, CATEGORY_COUNT>();
		std::vector<Chain *>				   chains					  = std::vector<Chain *>();
		std::vector<Residue *>				   residues					  = std::vector<Residue *>();
		std::vector<Atom *>					   atoms					  = std::vector<Atom *>();
		std::vector<Bond *>					   bonds					  = std::vector<Bond *>();
		size_t								   indexFirstBondExtraResidue = 0;

		Trajectory trajectory = Trajectory();

		// Unused in Core (Move to App ?)
		// uint _realChainCount = 0;
	};

} // namespace VTX::Core::Struct

#endif
