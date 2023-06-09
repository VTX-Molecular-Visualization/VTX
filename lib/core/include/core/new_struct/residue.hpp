#ifndef __VTX_CORE_STRUCT_RESIDUE__
#define __VTX_CORE_STRUCT_RESIDUE__

#include "_fwd.hpp"
#include "core/chemdb/residue.hpp"
#include "core/define.hpp"
#include <string>

namespace VTX::Core::Struct
{
	class Residue
	{
	  public:
		Chain * chainPtr	  = nullptr;
		size_t	internalIndex = INVALID_ID;

		ChemDB::Residue::SYMBOL symbol		   = ChemDB::Residue::SYMBOL::UNKNOWN;
		size_t					indexFirstAtom = INVALID_ID;
		size_t					atomCount	   = 0;
		size_t					indexFirstBond = INVALID_ID;
		size_t					bondCount	   = 0;

		// ChemDB::Residue::TYPE _type					= ChemDB::Residue::TYPE::STANDARD;
		// int _indexInOriginalChain = 0;
		// uint			   _realAtomCount  = 0;
		// ChemDB::Atom::TYPE _atomType	   = ChemDB::Atom::TYPE::NORMAL; // Set to solvent/ion only if full of it.
		// ChemDB::SecondaryStructure::TYPE _secondaryStructure = ChemDB::SecondaryStructure::TYPE::COIL;
		// char							 _insertionCode		 = ' ';
	};
} // namespace VTX::Core::Struct
#endif
