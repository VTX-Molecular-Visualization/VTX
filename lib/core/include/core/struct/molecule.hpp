#ifndef __VTX_CORE_STRUCT_MOLECULE__
#define __VTX_CORE_STRUCT_MOLECULE__

#include "_fwd.hpp"
#include "core/template/molecule.hpp"
#include "define.hpp"

namespace VTX::Core::Struct
{
	class Molecule : public MoleculeCore
	{
	  public:
		Molecule();
	};
} // namespace VTX::Core::Struct
#endif
