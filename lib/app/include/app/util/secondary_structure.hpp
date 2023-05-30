#ifndef __VTX_APP_UTIL_SECONDARY_STRUCTURE__
#define __VTX_APP_UTIL_SECONDARY_STRUCTURE__

#include "app/component/chemistry/_fwd.hpp"
#include <core/chemdb/secondary_structure.hpp>
#include <string>
#include <util/types.hpp>

namespace VTX::Util::App::SecondaryStructure
{
	namespace ChemDB = VTX::Core::ChemDB;

	void computeSecondaryStructure( VTX::App::Component::Chemistry::Molecule & p_molecule );
	void computeStride( VTX::App::Component::Chemistry::Molecule & p_molecule );

	ChemDB::SecondaryStructure::TYPE pdbFormattedToEnumSecondaryStructure( const std::string & p_str );
	std::string enumToPdbFormattedSecondaryStructure( const ChemDB::SecondaryStructure::TYPE p_enum );
} // namespace VTX::Util::App::SecondaryStructure

#endif
