#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"

namespace VTX::App::Component::Chemistry
{
	Chain::Chain( Molecule * const p_molecule, const size_t p_internalIndex ) :
		Core::Struct::Chain( p_molecule, p_internalIndex ) {};
} // namespace VTX::App::Component::Chemistry
