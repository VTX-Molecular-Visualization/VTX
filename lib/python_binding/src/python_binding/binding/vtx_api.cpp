#include "python_binding/binding/vtx_api.hpp"
#include "python_binding/api/api.hpp"
#include "python_binding/binding/binders/selection.hpp"
#include "python_binding/binding/helper.hpp"
#include <core/struct/system.hpp>
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <string>
#include <util/types.hpp>

// Skip automatic copy of vectors in pybind.
// PYBIND11_MAKE_OPAQUE( std::vector<VTX::App::API::Atom *> );

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

#ifdef JEVEUPA
		// Global
		p_apiModule.def( "getScene", []() { return SCENE(); }, pybind11::return_value_policy::reference );

		// Global
		p_apiModule.def(
			"setAtomName",
			[]( const std::string & p_systemName, const atom_index_t p_atomIndex, const std::string & p_name )
			{
				return SCENE()
					.getComponentByName<App::API::System>( p_systemName )
					.getAtom( p_atomIndex )
					->setName( p_name );
			}
		);

		// Selection
		Binding::Binders::bind_selection( p_apiModule );

		// Scene
		pybind11::class_<Application::Scene>( p_apiModule, "Scene", pybind11::module_local() )
			.def(
				"getSystem",
				[]( const Application::Scene & p_scene, const std::string & p_name ) -> const API::System &
				{ return p_scene.getComponentByName<API::System>( p_name ); },
				pybind11::return_value_policy::reference
			);
#endif // JEVEUPA

		pybind11::class_<VTX::Core::Struct::System>( p_apiModule, "SystemStruct", pybind11::module_local() )
			.def_property(
				"name",
				[]( const VTX::Core::Struct::System & p_system ) { return p_system.name; },
				[]( VTX::Core::Struct::System & p_system, const std::string & p_name ) { p_system.name = p_name; }
			);

		// Molecule
		pybind11::class_<API::System>( p_apiModule, "System", pybind11::module_local() )
			/*
			.def(
				"molStruct",
				&API::System::getSystemStruct,
				pybind11::return_value_policy::reference_internal
			)*/

			.def( "getName", &API::System::getName )
			.def( "setName", &API::System::setName )
			.def(
				"getAtoms", []( API::System & p_mol ) { return p_mol.getAtoms(); }, pybind11::return_value_policy::move
			)
			.def(
				"getAtoms",
				[]( const API::System & p_mol ) { return p_mol.getAtoms(); },
				pybind11::return_value_policy::move
			)
			.def(
				"getAtom",
				[]( const API::System & p_mol, const atom_index_t p_index ) { return p_mol.getAtom( p_index ); },
				pybind11::return_value_policy::move
			)
			.def(
				"getAtom",
				[]( API::System & p_mol, const atom_index_t p_index ) { return p_mol.getAtom( p_index ); },
				pybind11::return_value_policy::move
			);

		// Residue
		Helper::declareEnum<VTX::Core::ChemDB::Residue::SYMBOL>(
			p_apiModule, "RESIDUE_SYMBOL", pybind11::module_local()
		);

		// Atom
		Helper::declareEnum<VTX::Core::ChemDB::Atom::SYMBOL>( p_apiModule, "ATOM_SYMBOL", pybind11::module_local() );

		pybind11::class_<API::Atom>( p_apiModule, "Atom", pybind11::module_local() )
			.def( "getLocalPosition", []( const API::Atom & p_atom ) { return p_atom.getLocalPosition(); } )
			.def( "getWorldPosition", []( const API::Atom & p_atom ) { return p_atom.getWorldPosition(); } )
			.def( "getName", &API::Atom::getName )
			.def( "setName", &API::Atom::setName )
			.def( "getIndex", &API::Atom::getIndex )
			.def( "getSymbol", &API::Atom::getSymbol );
	}
} // namespace VTX::PythonBinding
