#ifndef __VTX_UTIL_MOLECULE__
#define __VTX_UTIL_MOLECULE__

// TODO VTX_LOGIC Remove from here
// #include "io/reader/residue_data_reader.hpp"
// #include "model/chemistry/enum/category_enum.hpp"
#include "model/chemistry/molecule.hpp"
#include <map>
#include <string>
#include <vector>

namespace VTX
{
	namespace Model
	{
		class Category;
		class Residue;
		class Chain;
		class Atom;
	} // namespace Model

	namespace Util::Molecule
	{
		// TODO VTX_LOGIC Remove from here
		// inline std::map<std::string, IO::Reader::ResidueData> mapLoadedResidueData
		//	= { { "", IO::Reader::ResidueData::DEFAULT } };

		// void									  loadResidueData( const std::string & p_residueSymbol );
		// const std::string &						  getResidueFullName( const std::string & p_residueSymbol );
		// const std::vector<IO::Reader::BondData> & getResidueBonds( const std::string & p_residueSymbol );
		// CATEGORY_ENUM							  getResidueCategory( const std::string & p_residueSymbol );

		// void recomputeBondOrders( Model::Molecule & p_molecule );
		// bool recomputeBondOrdersFromFile( Model::Molecule & p_molecule );

		void show( Model::Molecule & p_molecule,
				   const bool		 p_show,
				   const bool		 p_refreshMoleculeVisibility = true,
				   const bool		 p_notify					 = true );
		void show( Model::Category & p_category,
				   const bool		 p_show,
				   const bool		 p_showHierarchy			 = true,
				   const bool		 p_refreshMoleculeVisibility = true,
				   const bool		 p_notify					 = true );
		void show( Model::Chain & p_chain,
				   const bool	  p_show,
				   const bool	  p_showHierarchy			  = true,
				   const bool	  p_refreshMoleculeVisibility = true,
				   const bool	  p_notify					  = true );
		void show( Model::Residue & p_residue,
				   const bool		p_show,
				   const bool		p_showHierarchy				= true,
				   const bool		p_refreshMoleculeVisibility = true,
				   const bool		p_notify					= true );
		void show( Model::Atom & p_atom,
				   const bool	 p_show,
				   const bool	 p_showHierarchy			 = true,
				   const bool	 p_refreshMoleculeVisibility = true,
				   const bool	 p_notify					 = true );

		void solo( Model::Molecule & p_molecule, const bool p_refreshMoleculeVisibility = true );
		void solo( Model::Category & p_category, const bool p_refreshMoleculeVisibility = true );
		void soloCategories( Model::Molecule &					p_moleculeParent,
							 const std::vector<CATEGORY_ENUM> & p_categories,
							 const bool							p_refreshMoleculeVisibility = true );
		void solo( Model::Chain & p_chain, const bool p_refreshMoleculeVisibility = true );
		void soloChains( Model::Molecule &		   p_moleculeParent,
						 const std::vector<uint> & p_chainIndexes,
						 const bool				   p_refreshMoleculeVisibility = true );
		void solo( Model::Residue & p_residue, const bool p_refreshMoleculeVisibility = true );
		void soloResidues( Model::Molecule &		 p_moleculeParent,
						   const std::vector<uint> & p_residueIndexes,
						   const bool				 p_refreshMoleculeVisibility = true );
		void solo( Model::Atom & p_atom, const bool p_refreshMoleculeVisibility = true );
		void soloAtoms( Model::Molecule &		  p_moleculeParent,
						const std::vector<uint> & p_atomIndexes,
						const bool				  p_refreshMoleculeVisibility = true );

	} // namespace Util::Molecule
} // namespace VTX

#endif
