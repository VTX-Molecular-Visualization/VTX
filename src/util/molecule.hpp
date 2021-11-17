#ifndef __VTX_UTIL_MOLECULE__
#define __VTX_UTIL_MOLECULE__

#include "io/reader/residue_data_reader.hpp"
#include "setting.hpp"
#include <map>
#include <string>
#include <vector>

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class Chain;
		class Residue;
		class Atom;
	} // namespace Model

	namespace Util::Molecule
	{
		inline std::map<std::string, IO::Reader::ResidueData> mapLoadedResidueData
			= { { "", IO::Reader::ResidueData::DEFAULT } };

		void									  loadResidueData( const std::string & p_residueSymbol );
		const std::string &						  getResidueFullName( const std::string & p_residueSymbol );
		const std::vector<IO::Reader::BondData> & getResidueBonds( const std::string & p_residueSymbol );

		void recomputeBondOrders( Model::Molecule & p_molecule );
		bool recomputeBondOrdersFromFile( Model::Molecule & p_molecule );

		void show( Model::Molecule & p_molecule, const bool p_show, const bool p_refreshMoleculeVisibility = true );
		void show( Model::Chain & p_chain,
				   const bool	  p_show,
				   const bool	  p_showHierarchy			  = true,
				   const bool	  p_refreshMoleculeVisibility = true );
		void show( Model::Residue & p_residue,
				   const bool		p_show,
				   const bool		p_showHierarchy				= true,
				   const bool		p_refreshMoleculeVisibility = true );
		void show( Model::Atom & p_atom,
				   const bool	 p_show,
				   const bool	 p_showHierarchy			 = true,
				   const bool	 p_refreshMoleculeVisibility = true );

		void solo( Model::Molecule & p_molecule, const bool p_refreshMoleculeVisibility = true );
		void solo( Model::Chain & p_chain, const bool p_refreshMoleculeVisibility = true );
		void solo( Model::Residue & p_residue, const bool p_refreshMoleculeVisibility = true );
		void solo( Model::Atom & p_atom, const bool p_refreshMoleculeVisibility = true );

	} // namespace Util::Molecule
} // namespace VTX

#endif
