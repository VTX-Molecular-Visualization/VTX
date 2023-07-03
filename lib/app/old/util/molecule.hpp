#ifndef __VTX_APP_OLD_UTIL_MOLECULE__
#define __VTX_APP_OLD_UTIL_MOLECULE__

#include "app/old/application/setting.hpp"
#include "app/old/component/chemistry/_fwd.hpp"
#include "app/old/component/chemistry/molecule.hpp"
#include "app/old/internal/io/reader/residue_data_reader.hpp"
#include <core/chemdb/category.hpp>
#include <map>
#include <string>
#include <vector>

namespace VTX::Util::App::Old::Molecule
{
	inline std::map<std::string, VTX::App::Old::Internal::IO::Reader::ResidueData> mapLoadedResidueData
		= { { "", VTX::App::Old::Internal::IO::Reader::ResidueData::DEFAULT } };

	void				loadResidueData( const std::string & p_residueSymbol );
	const std::string & getResidueFullName( const std::string & p_residueSymbol );
	const std::vector<VTX::App::Old::Internal::IO::Reader::BondData> & getResidueBonds(
		const std::string & p_residueSymbol );
	VTX::Core::ChemDB::Category::TYPE getResidueCategory( const std::string & p_residueSymbol );

	void recomputeBondOrders( VTX::App::Old::Component::Chemistry::Molecule & p_molecule );
	bool recomputeBondOrdersFromFile( VTX::App::Old::Component::Chemistry::Molecule & p_molecule );

	void show( VTX::App::Old::Component::Chemistry::Molecule & p_molecule,
			   const bool								  p_show,
			   const bool								  p_refreshMoleculeVisibility = true,
			   const bool								  p_notify					  = true );
	void show( VTX::App::Old::Component::Chemistry::Category & p_category,
			   const bool								  p_show,
			   const bool								  p_showHierarchy			  = true,
			   const bool								  p_refreshMoleculeVisibility = true,
			   const bool								  p_notify					  = true );
	void show( VTX::App::Old::Component::Chemistry::Chain & p_chain,
			   const bool							   p_show,
			   const bool							   p_showHierarchy			   = true,
			   const bool							   p_refreshMoleculeVisibility = true,
			   const bool							   p_notify					   = true );
	void show( VTX::App::Old::Component::Chemistry::Residue & p_residue,
			   const bool								 p_show,
			   const bool								 p_showHierarchy			 = true,
			   const bool								 p_refreshMoleculeVisibility = true,
			   const bool								 p_notify					 = true );
	void show( VTX::App::Old::Component::Chemistry::Atom & p_atom,
			   const bool							  p_show,
			   const bool							  p_showHierarchy			  = true,
			   const bool							  p_refreshMoleculeVisibility = true,
			   const bool							  p_notify					  = true );

	void solo( VTX::App::Old::Component::Chemistry::Molecule & p_molecule, const bool p_refreshMoleculeVisibility = true );
	void solo( VTX::App::Old::Component::Chemistry::Category & p_category, const bool p_refreshMoleculeVisibility = true );
	void soloCategories( VTX::App::Old::Component::Chemistry::Molecule &				p_moleculeParent,
						 const std::vector<VTX::Core::ChemDB::Category::TYPE> & p_categories,
						 const bool												p_refreshMoleculeVisibility = true );
	void solo( VTX::App::Old::Component::Chemistry::Chain & p_chain, const bool p_refreshMoleculeVisibility = true );
	void soloChains( VTX::App::Old::Component::Chemistry::Molecule & p_moleculeParent,
					 const std::vector<uint> &					p_chainIndexes,
					 const bool									p_refreshMoleculeVisibility = true );
	void solo( VTX::App::Old::Component::Chemistry::Residue & p_residue, const bool p_refreshMoleculeVisibility = true );
	void soloResidues( VTX::App::Old::Component::Chemistry::Molecule & p_moleculeParent,
					   const std::vector<uint> &				  p_residueIndexes,
					   const bool								  p_refreshMoleculeVisibility = true );
	void solo( VTX::App::Old::Component::Chemistry::Atom & p_atom, const bool p_refreshMoleculeVisibility = true );
	void soloAtoms( VTX::App::Old::Component::Chemistry::Molecule & p_moleculeParent,
					const std::vector<uint> &				   p_atomIndexes,
					const bool								   p_refreshMoleculeVisibility = true );

} // namespace VTX::Util::App::Old::Molecule

#endif
