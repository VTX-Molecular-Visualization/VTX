#include "core/struct/secondary_structure.hpp"
#include "core/chemDB/color.hpp"
#include "core/chemDB/secondary_structure.hpp"
#include "core/struct/chain.hpp"
#include "core/struct/molecule.hpp"
#include "core/struct/residue.hpp"
#include <util/chrono.hpp>
#include <util/logger.hpp>

namespace VTX::Core::Struct
{
	SecondaryStructure::SecondaryStructure( Molecule * const p_molecule ) : _molecule( p_molecule ) {}

} // namespace VTX::Core::Struct
