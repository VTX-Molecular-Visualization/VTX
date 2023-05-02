#ifndef __VTX_APP_UTIL_MOLECULE__
#define __VTX_APP_UTIL_MOLECULE__

#include "app/application/setting.hpp"
#include "app/component/chemistry/_fwd.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/internal/chemdb/category.hpp"
#include "app/internal/io/reader/residue_data_reader.hpp"
#include <map>
#include <string>
#include <vector>

namespace VTX::Util::App::Molecule
{
	inline std::map<std::string, VTX::App::Internal::IO::Reader::ResidueData> mapLoadedResidueData
		= { { "", VTX::App::Internal::IO::Reader::ResidueData::DEFAULT } };

	void				loadResidueData( const std::string & p_residueSymbol );
	const std::string & getResidueFullName( const std::string & p_residueSymbol );
	const std::vector<VTX::App::Internal::IO::Reader::BondData> & getResidueBonds(
		const std::string & p_residueSymbol );
	VTX::App::Internal::ChemDB::Category::TYPE getResidueCategory( const std::string & p_residueSymbol );

	void recomputeBondOrders( VTX::App::Component::Chemistry::Molecule & p_molecule );
	bool recomputeBondOrdersFromFile( VTX::App::Component::Chemistry::Molecule & p_molecule );

	void show( VTX::App::Component::Chemistry::Molecule & p_molecule,
			   const bool								  p_show,
			   const bool								  p_refreshMoleculeVisibility = true,
			   const bool								  p_notify					  = true );
	void show( VTX::App::Component::Chemistry::Category & p_category,
			   const bool								  p_show,
			   const bool								  p_showHierarchy			  = true,
			   const bool								  p_refreshMoleculeVisibility = true,
			   const bool								  p_notify					  = true );
	void show( VTX::App::Component::Chemistry::Chain & p_chain,
			   const bool							   p_show,
			   const bool							   p_showHierarchy			   = true,
			   const bool							   p_refreshMoleculeVisibility = true,
			   const bool							   p_notify					   = true );
	void show( VTX::App::Component::Chemistry::Residue & p_residue,
			   const bool								 p_show,
			   const bool								 p_showHierarchy			 = true,
			   const bool								 p_refreshMoleculeVisibility = true,
			   const bool								 p_notify					 = true );
	void show( VTX::App::Component::Chemistry::Atom & p_atom,
			   const bool							  p_show,
			   const bool							  p_showHierarchy			  = true,
			   const bool							  p_refreshMoleculeVisibility = true,
			   const bool							  p_notify					  = true );

	void solo( VTX::App::Component::Chemistry::Molecule & p_molecule, const bool p_refreshMoleculeVisibility = true );
	void solo( VTX::App::Component::Chemistry::Category & p_category, const bool p_refreshMoleculeVisibility = true );
	void soloCategories( VTX::App::Component::Chemistry::Molecule &						 p_moleculeParent,
						 const std::vector<VTX::App::Internal::ChemDB::Category::TYPE> & p_categories,
						 const bool p_refreshMoleculeVisibility = true );
	void solo( VTX::App::Component::Chemistry::Chain & p_chain, const bool p_refreshMoleculeVisibility = true );
	void soloChains( VTX::App::Component::Chemistry::Molecule & p_moleculeParent,
					 const std::vector<uint> &					p_chainIndexes,
					 const bool									p_refreshMoleculeVisibility = true );
	void solo( VTX::App::Component::Chemistry::Residue & p_residue, const bool p_refreshMoleculeVisibility = true );
	void soloResidues( VTX::App::Component::Chemistry::Molecule & p_moleculeParent,
					   const std::vector<uint> &				  p_residueIndexes,
					   const bool								  p_refreshMoleculeVisibility = true );
	void solo( VTX::App::Component::Chemistry::Atom & p_atom, const bool p_refreshMoleculeVisibility = true );
	void soloAtoms( VTX::App::Component::Chemistry::Molecule & p_moleculeParent,
					const std::vector<uint> &				   p_atomIndexes,
					const bool								   p_refreshMoleculeVisibility = true );

} // namespace VTX::Util::App::Molecule

#endif
