#include "python_binding/binding/vtx_api.hpp"
#include "python_binding/binding/binders/selection.hpp"
#include "python_binding/binding/helper.hpp"
#include <app/application/scene.hpp>
#include <app/application/system.hpp>
#include <app/component/chemistry/atom.hpp>
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/component/chemistry/residue.hpp>
#include <app/vtx_app.hpp>
#include <core/struct/molecule.hpp>
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <string>
#include <util/types.hpp>

// Skip automatic copy of vectors in pybind.
PYBIND11_MAKE_OPAQUE( std::vector<VTX::App::Component::Chemistry::Atom *> );

namespace VTX::PythonBinding
{
	void apiModule( pybind11::module_ & p_apiModule )
	{
		using namespace App;

		p_apiModule.doc() = "VTX API module."; // optional module docstring

		// Check PYBIND11_MAKE_OPAQUE

		// Util
		pybind11::class_<Vec3f>( p_apiModule, "Vec3f", pybind11::module_local() )
			.def_readwrite( "x", &Vec3f::x )
			.def_readwrite( "y", &Vec3f::y )
			.def_readwrite( "z", &Vec3f::z );

		// Global
		p_apiModule.def(
			"getScene", []() { return VTXApp::get().getScene(); }, pybind11::return_value_policy::reference
		);

		// Selection
		Binding::Binders::bind_selection( p_apiModule );

		// Scene
		pybind11::class_<Application::Scene>( p_apiModule, "Scene", pybind11::module_local() )
			.def(
				"getMolecule",
				[]( const Application::Scene & p_scene,
					const std::string &		   p_name ) -> const Component::Chemistry::Molecule &
				{ return p_scene.getComponentByName<Component::Chemistry::Molecule>( p_name ); },
				pybind11::return_value_policy::reference
			);

		pybind11::class_<VTX::Core::Struct::Molecule>( p_apiModule, "MoleculeStruct", pybind11::module_local() )
			.def_property(
				"name",
				[]( const VTX::Core::Struct::Molecule & p_molecule ) { return p_molecule.name; },
				[]( VTX::Core::Struct::Molecule & p_molecule, const std::string & p_name ) { p_molecule.name = p_name; }
			);

		// Molecule
		pybind11::class_<Component::Chemistry::Molecule>( p_apiModule, "Molecule", pybind11::module_local() )
			.def(
				"molStruct",
				&Component::Chemistry::Molecule::getMoleculeStruct,
				pybind11::return_value_policy::reference_internal
			)
			.def( "getName", &Component::Chemistry::Molecule::getName )
			.def( "setName", &Component::Chemistry::Molecule::setName )
			.def(
				"getAtoms",
				[]( Component::Chemistry::Molecule & p_mol ) { return &p_mol.getAtoms(); },
				pybind11::return_value_policy::reference_internal
			)
			.def(
				"getAtoms",
				[]( const Component::Chemistry::Molecule & p_mol ) { return &p_mol.getAtoms(); },
				pybind11::return_value_policy::reference_internal
			)
			.def(
				"getAtom",
				[]( const Component::Chemistry::Molecule & p_mol, const atom_index_t p_index )
				{ return *p_mol.getAtom( p_index ); },
				pybind11::return_value_policy::reference
			)
			.def(
				"getAtom",
				[]( Component::Chemistry::Molecule & p_mol, const atom_index_t p_index )
				{ return *p_mol.getAtom( p_index ); },
				pybind11::return_value_policy::reference
			);

		// Residue
		Helper::declareEnum<VTX::Core::ChemDB::Residue::SYMBOL>( p_apiModule, "RESIDUE_SYMBOL" );

		// Atom
		Helper::declareEnum<VTX::Core::ChemDB::Atom::SYMBOL>( p_apiModule, "ATOM_SYMBOL" );

		pybind11::class_<Component::Chemistry::Atom>( p_apiModule, "Atom", pybind11::module_local() )
			.def(
				"getLocalPosition",
				[]( const Component::Chemistry::Atom & p_atom ) { return p_atom.getLocalPosition(); }
			)
			.def(
				"getWorldPosition",
				[]( const Component::Chemistry::Atom & p_atom ) { return p_atom.getWorldPosition(); }
			)
			.def( "getName", &Component::Chemistry::Atom::getName )
			.def( "setName", &Component::Chemistry::Atom::setName )
			.def( "getIndex", &Component::Chemistry::Atom::getIndex )
			.def( "getSymbol", &Component::Chemistry::Atom::getSymbol );

		// Global
		p_apiModule.def(
			"setAtomName",
			[]( const std::string & p_moleculeName, const atom_index_t p_atomIndex, const std::string & p_name )
			{
				return VTXApp::get()
					.getScene()
					.getComponentByName<App::Component::Chemistry::Molecule>( p_moleculeName )
					.getAtom( p_atomIndex )
					->setName( p_name );
			}
		);
	}
} // namespace VTX::PythonBinding
