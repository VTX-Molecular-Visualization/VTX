#ifndef __VTX_CORE_STRUCT_BOND__
#define __VTX_CORE_STRUCT_BOND__

#include "_fwd.hpp"
#include "core/chemdb/bond.hpp"
#include "core/define.hpp"

namespace VTX::Core::Struct
{
	class Bond
	{
	  public:
		size_t internalIndex = INVALID_INDEX;

		Molecule *			moleculePtr = nullptr;
		ChemDB::Bond::ORDER order		= ChemDB::Bond::ORDER::UNKNOWN;

		size_t getIndexFirstAtom() const;
		void   setIndexFirstAtom( const size_t p_atomIndex );
		size_t getIndexSecondAtom() const;
		void   setIndexSecondAtom( const size_t p_atomIndex );
	};

} // namespace VTX::Core::Struct
#endif
