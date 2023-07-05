#include "app/component/chemistry/residue.hpp"
#include "app/component/chemistry/chain.hpp"

namespace VTX::App::Component::Chemistry
{
	Residue::Residue( const size_t p_index ) : Core::Struct::Residue( p_index ) {}
	Residue::Residue( Chain * const p_chain, const size_t p_index ) : Core::Struct::Residue( p_chain, p_index ) {}
} // namespace VTX::App::Component::Chemistry
