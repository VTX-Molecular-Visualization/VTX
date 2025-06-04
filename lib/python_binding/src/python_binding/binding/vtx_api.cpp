#include "python_binding/binding/vtx_api.hpp"
#include "python_binding/api/api.hpp"
#include "python_binding/binding/helper.hpp"
#include "python_binding/interpretor.hpp"
#include <core/struct/system.hpp>
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <string>
#include <util/types.hpp>

namespace VTX::PythonBinding::Binding
{
	template<typename ITEM>
	inline void registerCollection( pybind11::module_ & p_apiModule, const char * p_pythonClassName )
	{
		pybind11::class_<API::Collection<ITEM>>( p_apiModule, p_pythonClassName, pybind11::module_local() )
			.def(
				"__iter__",
				[]( API::Collection<ITEM> & c ) { return pybind11::make_iterator( c.begin(), c.end() ); },
				pybind11::keep_alive<0, 1>()
			)
			//.def(
			//	"__getitem__",
			//	(ITEM ( API::Collection<ITEM>::* )( const size_t & ))&API::Collection<ITEM>::operator[],
			//	pybind11::keep_alive<0, 1>()
			//)
			.def(
				"__getitem__",
				[]( API::Collection<ITEM> & _, const size_t & idx )
				{
					if ( _.size() > idx )
						return _[ idx ];
					else
						throw pybind11::index_error(
							"Provided index is greater than or equal to the size of the list."
						);
				}
			)
			.def(
				"__len__",
				(size_t ( API::Collection<ITEM>::* )())&API::Collection<ITEM>::size,
				pybind11::keep_alive<0, 1>()
			)

			;
	}

	void applyVtxLocalCommandBinding( pybind11::module_ & p_commandModule )
	{
		p_commandModule.def( "runScript", []( const std::string & r ) { INTERPRETOR().runScript( r ); } );
	}
	void applyVtxApiBinding( pybind11::module_ & p_apiModule )
	{
		// Check PYBIND11_MAKE_OPAQUE
		// Util
		pybind11::class_<Vec3f>( p_apiModule, "Vec3f", pybind11::module_local() )
			.def( pybind11::init<>() )
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
		// Collections
		registerCollection<API::Atom>( p_apiModule, "CollectionAtom" );
		registerCollection<API::Residue>( p_apiModule, "CollectionResidue" );
		registerCollection<API::Chain>( p_apiModule, "CollectionChain" );
		registerCollection<API::System>( p_apiModule, "CollectionSystem" );

		// System
		pybind11::class_<API::System>( p_apiModule, "System", pybind11::module_local() )
			.def( "getName", &API::System::getName )
			.def( "setName", &API::System::setName )
			.def( "getChains", []( API::System & p_mol ) { return p_mol.getChains(); } )
			.def( "getChains", []( const API::System & p_mol ) { return p_mol.getChains(); } )
			.def( "getAtoms", []( API::System & p_mol ) { return p_mol.getAtoms(); } )
			.def( "getAtoms", []( const API::System & p_mol ) { return p_mol.getAtoms(); } )
			.def(
				"getChain",
				[]( const API::System & p_mol, const size_t & p_index ) { return p_mol.getChain( p_index ); }
			)
			.def( "getChain", []( API::System & p_mol, const size_t & p_index ) { return p_mol.getChain( p_index ); } )
			.def(
				"getAtom",
				[]( const API::System & p_mol, const atom_index_t p_index ) { return p_mol.getAtom( p_index ); }
			)
			.def(
				"getAtom", []( API::System & p_mol, const atom_index_t p_index ) { return p_mol.getAtom( p_index ); }
			)
			.def( "initAtoms", &API::System::initAtoms )
			.def( "initBonds", &API::System::initBonds )
			.def( "initChains", &API::System::initChains )
			.def( "initResidues", &API::System::initResidues )
			.def( "getRealAtomCount", &API::System::getRealAtomCount )
			.def( "getRealChainCount", &API::System::getRealChainCount )
			.def( "getRealResidueCount", &API::System::getRealResidueCount )
			.def( "getResidues", []( const API::System & p_mol ) { return p_mol.getResidues(); } )
			.def( "getResidues", []( API::System & p_mol ) { return p_mol.getResidues(); } )
			.def(
				"getResidue",
				[]( const API::System & p_mol, const size_t & p_index ) { return p_mol.getResidue( p_index ); }
			)
			.def(
				"getResidue", []( API::System & p_mol, const size_t & p_index ) { return p_mol.getResidue( p_index ); }
			)

			;

		// Residue
		Helper::declareEnum<VTX::Core::ChemDB::Residue::SYMBOL>(
			p_apiModule, "RESIDUE_SYMBOL", pybind11::module_local()
		);

		pybind11::class_<API::Residue>( p_apiModule, "Residue", pybind11::module_local() )
			.def( "getShortName", &API::Residue::getShortName )
			.def( "getName", &API::Residue::getName )
			.def( "getLongName", &API::Residue::getLongName )
			.def( "getIndex", &API::Residue::getIndex )
			.def( "setIndex", &API::Residue::setIndex )
			.def( "getIndexFirstAtom", &API::Residue::getIndexFirstAtom )
			.def( "setIndexFirstAtom", &API::Residue::setIndexFirstAtom )
			.def( "getIndexLastAtom", &API::Residue::getIndexLastAtom )
			.def( "getIndexLastAtom", &API::Residue::getIndexLastAtom )
			.def( "getAtomCount", &API::Residue::getAtomCount )
			.def( "setAtomCount", &API::Residue::setAtomCount )
			.def( "getIndexFirstBond", &API::Residue::getIndexFirstBond )
			.def( "setIndexFirstBond", &API::Residue::setIndexFirstBond )
			.def( "getBondCount", &API::Residue::getBondCount )
			.def( "setBondCount", &API::Residue::setBondCount )
			.def( "getIndexInOriginalChain", &API::Residue::getIndexInOriginalChain )
			.def( "setIndexInOriginalChain", &API::Residue::setIndexInOriginalChain )
			.def( "setVisible", &API::Residue::setVisible )
			.def( "isVisible", &API::Residue::isVisible )
			.def( "isFullyVisible", &API::Residue::isFullyVisible )
			.def(
				"getChain", []( API::Residue & r ) { return r.getChain(); }, pybind11::return_value_policy::move
			)
			.def(
				"getSystem", []( API::Residue & r ) { return r.getSystem(); }, pybind11::return_value_policy::move
			)

			;

		// Atom
		Helper::declareEnum<VTX::Core::ChemDB::Atom::SYMBOL>( p_apiModule, "ATOM_SYMBOL", pybind11::module_local() );
		Helper::declareEnum<VTX::Core::ChemDB::Atom::TYPE>( p_apiModule, "ATOM_TYPE", pybind11::module_local() );

		pybind11::class_<API::Atom>( p_apiModule, "Atom", pybind11::module_local() )
			.def( "getLocalPosition", &API::Atom::getLocalPosition )
			.def( "getWorldPosition", &API::Atom::getWorldPosition )
			.def( "getIndex", &API::Atom::getIndex )
			.def( "setIndex", &API::Atom::setIndex )
			.def( "getName", &API::Atom::getName )
			.def( "setName", &API::Atom::setName )
			.def( "getSymbol", &API::Atom::getSymbol )
			.def( "setSymbol", &API::Atom::setSymbol )
			.def( "getType", &API::Atom::getType )
			.def( "setType", &API::Atom::setType )
			.def( "getVdwRadius", &API::Atom::getVdwRadius )
			.def( "isVisible", &API::Atom::isVisible )
			.def( "setVisible", &API::Atom::setVisible )
			.def( "remove", &API::Atom::remove );

		pybind11::class_<API::Chain>( p_apiModule, "Chain", pybind11::module_local() )
			.def( "getIndex", &API::Chain::getIndex )
			.def( "setIndex", &API::Chain::setIndex )
			.def( "getName", &API::Chain::getName )
			.def( "setName", &API::Chain::setName )
			.def( "getIndexFirstResidue", &API::Chain::getIndexFirstResidue )
			.def( "getIndexLastResidue", &API::Chain::getIndexLastResidue )
			.def( "setIndexFirstResidue", &API::Chain::setIndexFirstResidue )
			.def( "getResidueCount", &API::Chain::getResidueCount )
			.def( "setResidueCount", &API::Chain::setResidueCount )
			.def( "getIndexFirstAtom", &API::Chain::getIndexFirstAtom )
			.def( "getIndexLastAtom", &API::Chain::getIndexLastAtom )
			.def( "getOriginalChainID", &API::Chain::getOriginalChainID )
			.def( "setOriginalChainID", &API::Chain::setOriginalChainID )
			.def( "isVisible", &API::Chain::isVisible )
			.def( "isFullyVisible", &API::Chain::isFullyVisible )
			.def( "setVisible", &API::Chain::setVisible )
			.def( "remove", &API::Chain::remove );
	}
} // namespace VTX::PythonBinding::Binding
