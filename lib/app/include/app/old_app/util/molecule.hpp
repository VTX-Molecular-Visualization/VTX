#ifndef __VTX_UTIL_MOLECULE__
#define __VTX_UTIL_MOLECULE__

#include "app/component/chemistry/_fwd.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/internal/chemdb/category.hpp"
#include "app/old_app/io/reader/residue_data_reader.hpp"
#include "app/old_app/setting.hpp"
#include <map>
#include <string>
#include <vector>

namespace VTX::Util::Molecule
{
	inline std::map<std::string, IO::Reader::ResidueData> mapLoadedResidueData
		= { { "", IO::Reader::ResidueData::DEFAULT } };

	void									  loadResidueData( const std::string & p_residueSymbol );
	const std::string &						  getResidueFullName( const std::string & p_residueSymbol );
	const std::vector<IO::Reader::BondData> & getResidueBonds( const std::string & p_residueSymbol );
	App::Internal::ChemDB::Category::TYPE	  getResidueCategory( const std::string & p_residueSymbol );

	void recomputeBondOrders( App::Component::Chemistry::Molecule & p_molecule );
	bool recomputeBondOrdersFromFile( App::Component::Chemistry::Molecule & p_molecule );

	void show( App::Component::Chemistry::Molecule & p_molecule,
			   const bool							 p_show,
			   const bool							 p_refreshMoleculeVisibility = true,
			   const bool							 p_notify					 = true );
	void show( App::Component::Chemistry::Category & p_category,
			   const bool							 p_show,
			   const bool							 p_showHierarchy			 = true,
			   const bool							 p_refreshMoleculeVisibility = true,
			   const bool							 p_notify					 = true );
	void show( App::Component::Chemistry::Chain & p_chain,
			   const bool						  p_show,
			   const bool						  p_showHierarchy			  = true,
			   const bool						  p_refreshMoleculeVisibility = true,
			   const bool						  p_notify					  = true );
	void show( App::Component::Chemistry::Residue & p_residue,
			   const bool							p_show,
			   const bool							p_showHierarchy				= true,
			   const bool							p_refreshMoleculeVisibility = true,
			   const bool							p_notify					= true );
	void show( App::Component::Chemistry::Atom & p_atom,
			   const bool						 p_show,
			   const bool						 p_showHierarchy			 = true,
			   const bool						 p_refreshMoleculeVisibility = true,
			   const bool						 p_notify					 = true );

	void solo( App::Component::Chemistry::Molecule & p_molecule, const bool p_refreshMoleculeVisibility = true );
	void solo( App::Component::Chemistry::Category & p_category, const bool p_refreshMoleculeVisibility = true );
	void soloCategories( App::Component::Chemistry::Molecule &						p_moleculeParent,
						 const std::vector<App::Internal::ChemDB::Category::TYPE> & p_categories,
						 const bool p_refreshMoleculeVisibility = true );
	void solo( App::Component::Chemistry::Chain & p_chain, const bool p_refreshMoleculeVisibility = true );
	void soloChains( App::Component::Chemistry::Molecule & p_moleculeParent,
					 const std::vector<uint> &			   p_chainIndexes,
					 const bool							   p_refreshMoleculeVisibility = true );
	void solo( App::Component::Chemistry::Residue & p_residue, const bool p_refreshMoleculeVisibility = true );
	void soloResidues( App::Component::Chemistry::Molecule & p_moleculeParent,
					   const std::vector<uint> &			 p_residueIndexes,
					   const bool							 p_refreshMoleculeVisibility = true );
	void solo( App::Component::Chemistry::Atom & p_atom, const bool p_refreshMoleculeVisibility = true );
	void soloAtoms( App::Component::Chemistry::Molecule & p_moleculeParent,
					const std::vector<uint> &			  p_atomIndexes,
					const bool							  p_refreshMoleculeVisibility = true );

} // namespace VTX::Util::Molecule

#endif
