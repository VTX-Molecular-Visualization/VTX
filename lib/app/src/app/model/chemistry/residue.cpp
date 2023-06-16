#include "app/model/chemistry/residue.hpp"

namespace VTX::App::Model::Chemistry
{
	Residue::Residue( const size_t p_index ) : ResidueCore( p_index ) {}
	Residue::Residue( Chain * const p_chain, const size_t p_index ) : ResidueCore( p_chain, p_index ) {}
} // namespace VTX::App::Model::Chemistry
