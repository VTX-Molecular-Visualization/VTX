#ifndef __VTX_UTIL_MOLECULE__
#define __VTX_UTIL_MOLECULE__

#include "io/reader/residue_data_reader.hpp"
#include <map>
#include <string>
#include <vector>

namespace VTX
{
	namespace Model
	{
		class Molecule;
	}

	namespace Util::Molecule
	{
		inline std::map<std::string, IO::Reader::ResidueData> mapLoadedResidueData
			= std::map<std::string, IO::Reader::ResidueData>();

		void									  loadResidueData( const std::string & p_residueSymbol );
		const std::string &						  getResidueFullName( const std::string & p_residueSymbol );
		const std::vector<IO::Reader::BondData> & getResidueBonds( const std::string & p_residueSymbol );

		void recomputeBondOrders( Model::Molecule & p_molecule );
		void recomputeBondOrdersFromFile( Model::Molecule & p_molecule );

	} // namespace Util::Molecule
} // namespace VTX

#endif
