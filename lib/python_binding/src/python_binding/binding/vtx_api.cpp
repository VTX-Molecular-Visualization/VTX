#include "python_binding/binding/vtx_api.hpp"
#include "python_binding/api/api.hpp"
#include "python_binding/binding/helper.hpp"
#include "python_binding/interpretor.hpp"
#include "python_binding/log_redirection.hpp"
#include <core/struct/topology.hpp>
#include <memory>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <string>
#include <util/color/rgba.hpp>
#include <util/math/aabb.hpp>
#include <util/math/transform.hpp>
#include <util/resolution.hpp>
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
			.def(
				"__getitem__",
				[]( API::Collection<ITEM> & _, const Index & idx )
				{
					if ( _.size() > idx )
					{
						return _[ idx ];
					}
					else
					{
						throw pybind11::index_error(
							"Provided index is greater than or equal to the size of the list."
						); // At first I put it in the Collection operator[] method directly but I couldn't catch the
						   // exception from the interpretor code. So I putted it here instead and it works.
					}
				}
			)
			.def( "__len__", (size_t ( API::Collection<ITEM>::* )())&API::Collection<ITEM>::size )

			;
	}

	void applyVtxLocalCommandBinding( VTX::PythonBinding::Interpretor & p_interpretor )
	{
		pybind11::module_ * commandModule = nullptr;
		p_interpretor.getPythonModule( &commandModule );
		commandModule->def(
			"runScript", [ &interpretor = p_interpretor ]( const std::string & r ) { interpretor.runScript( r ); }
		);
	}

	void applyModuleCustomization( pybind11::module_ & p_module )
	{
		p_module.doc()
			= "VTX Python module. Will contain every functions and classes provided by VTX to interact with the "
			  "renderer and its underlying data. Please use the function help([Anything]) on any element of the module "
			  "to get help message."; // optional module docstring

		// Class to redirect Python prints
		pybind11::class_<LogRedirection>( p_module, "LogRedirection", pybind11::module_local() )
			.def_static( "write", &LogRedirection::write )
			.def_static( "flush", &LogRedirection::flush );

		Binding::applyVtxApiBinding( p_module );
	}

	void applyVtxApiBinding( pybind11::module_ & p_apiModule )
	{
		// Check PYBIND11_MAKE_OPAQUE
		// Util
		pybind11::class_<Vec2f>( p_apiModule, "Vec2f", pybind11::module_local() )
			.def( pybind11::init<>() )
			.def(
				pybind11::init( []( const float p_x, const float p_y ) { return Vec2f( p_x, p_y ); } ),
				pybind11::arg( "x" ),
				pybind11::arg( "y" )
			)
			.def_readwrite( "x", &Vec2f::x )
			.def_readwrite( "y", &Vec2f::y );

		pybind11::class_<Vec3f>( p_apiModule, "Vec3f", pybind11::module_local() )
			.def( pybind11::init<>() )
			.def(
				pybind11::init( []( const float p_x, const float p_y, const float p_z )
								{ return Vec3f( p_x, p_y, p_z ); } ),
				pybind11::arg( "x" ),
				pybind11::arg( "y" ),
				pybind11::arg( "z" )
			)
			.def_readwrite( "x", &Vec3f::x )
			.def_readwrite( "y", &Vec3f::y )
			.def_readwrite( "z", &Vec3f::z );

		pybind11::class_<Vec4f>( p_apiModule, "Vec4f", pybind11::module_local() )
			.def( pybind11::init<>() )
			.def(
				pybind11::init( []( const float p_x, const float p_y, const float p_z, const float p_w )
								{ return Vec4f( p_x, p_y, p_z, p_w ); } ),
				pybind11::arg( "x" ),
				pybind11::arg( "y" ),
				pybind11::arg( "z" ),
				pybind11::arg( "w" )
			)
			.def_readwrite( "x", &Vec4f::x )
			.def_readwrite( "y", &Vec4f::y )
			.def_readwrite( "z", &Vec4f::z )
			.def_readwrite( "w", &Vec4f::w );

		pybind11::class_<Quatf>( p_apiModule, "Quatf", pybind11::module_local() )
			.def( pybind11::init<>() )
			.def(
				pybind11::init( []( const float p_x, const float p_y, const float p_z )
								{ return Quatf( 1.f, p_x, p_y, p_z ); } ),
				pybind11::arg( "x" ),
				pybind11::arg( "y" ),
				pybind11::arg( "z" )
			)
			.def_readwrite( "x", &Quatf::x )
			.def_readwrite( "y", &Quatf::y )
			.def_readwrite( "z", &Quatf::z )
			.def_readwrite( "w", &Quatf::w );

		using Transform = Util::Math::Transform;
		pybind11::class_<Transform>( p_apiModule, "Transform", pybind11::module_local() )
			.def( pybind11::init<>() )
			.def_property(
				"position",
				[]( const Transform & p_transform ) { return p_transform.getPosition(); },
				[]( Transform & p_transform, const Vec3f & p_position ) { p_transform.setPosition( p_position ); }
			)
			.def_property(
				"rotation",
				[]( const Transform & p_transform ) { return p_transform.getRotation(); },
				[]( Transform & p_transform, const Quatf & p_rotation ) { p_transform.setRotation( p_rotation ); }
			)
			.def_property(
				"scale",
				[]( const Transform & p_transform ) { return p_transform.getScale(); },
				[]( Transform & p_transform, const Vec3f & p_scale ) { p_transform.setScale( p_scale ); }
			);

		pybind11::class_<Util::Resolution>( p_apiModule, "Resolution", pybind11::module_local() )
			.def(
				pybind11::init( []( const std::string & p_name, const size_t p_width, const size_t p_height )
								{ return Util::Resolution { p_name, p_width, p_height }; } ),
				pybind11::arg( "name" ),
				pybind11::arg( "width" ),
				pybind11::arg( "height" )
			)
			.def_readwrite( "name", &Util::Resolution::name )
			.def_readwrite( "width", &Util::Resolution::width )
			.def_readwrite( "height", &Util::Resolution::height );

		pybind11::class_<Util::Color::Rgba>( p_apiModule, "Rgba", pybind11::module_local() )
			.def( pybind11::init<>() )
			.def(
				pybind11::init( []( const float p_r, const float p_g, const float p_b, const float p_a )
								{ return Util::Color::Rgba( p_r, p_g, p_b, p_a ); } ),
				pybind11::arg( "r" ),
				pybind11::arg( "g" ),
				pybind11::arg( "b" ),
				pybind11::arg( "a" ) = 1.f
			)
			.def_property(
				"r",
				[]( const Util::Color::Rgba & p_color ) { return p_color.r(); },
				[]( Util::Color::Rgba & p_color, const float p_value ) { p_color.r() = p_value; }
			)
			.def_property(
				"g",
				[]( const Util::Color::Rgba & p_color ) { return p_color.g(); },
				[]( Util::Color::Rgba & p_color, const float p_value ) { p_color.g() = p_value; }
			)
			.def_property(
				"b",
				[]( const Util::Color::Rgba & p_color ) { return p_color.b(); },
				[]( Util::Color::Rgba & p_color, const float p_value ) { p_color.b() = p_value; }
			)
			.def_property(
				"a",
				[]( const Util::Color::Rgba & p_color ) { return p_color.a(); },
				[]( Util::Color::Rgba & p_color, const float p_value ) { p_color.a() = p_value; }
			);

		using Range = Core::Struct::IndexRange;
		pybind11::class_<Range>( p_apiModule, "Range", pybind11::module_local() )
			.def( pybind11::init<>() )
			.def( pybind11::init<const Index>(), pybind11::arg( "value" ) )
			.def( pybind11::init<const Index, const Index>(), pybind11::arg( "first" ), pybind11::arg( "last" ) )
			.def_property_readonly( "first", &Range::getFirst )
			.def_property_readonly( "last", &Range::getLast )
			.def_property_readonly( "count", &Range::getCount );

		using RangeList = Core::Struct::IndexRangeList;
		pybind11::class_<RangeList>( p_apiModule, "RangeList", pybind11::module_local() )
			.def( pybind11::init<>() )
			.def( pybind11::init<const Index>(), pybind11::arg( "value" ) )
			.def( pybind11::init<const Range &>(), pybind11::arg( "range" ) )
			.def(
				pybind11::init(
					[]( const std::vector<Index> & p_values )
					{
						RangeList result;
						result.addValues( p_values );
						return result;
					}
				),
				pybind11::arg( "values" )
			)
			.def(
				pybind11::init(
					[]( const std::vector<Range> & p_ranges )
					{
						RangeList result;
						for ( const Range & range : p_ranges )
						{
							result.addRange( range );
						}
						return result;
					}
				),
				pybind11::arg( "ranges" )
			)
			.def_property_readonly( "count", &RangeList::count )
			.def_property_readonly( "rangeCount", &RangeList::rangeCount )
			.def_property_readonly(
				"ranges",
				[]( const RangeList & p_ranges )
				{ return std::vector<Range>( p_ranges.rangeBegin(), p_ranges.rangeEnd() ); }
			);

		using AABB = Util::Math::AABB;
		pybind11::class_<AABB>( p_apiModule, "AABB", pybind11::module_local() )
			.def( pybind11::init<>() )
			.def( pybind11::init<const Vec3f &>(), pybind11::arg( "point" ) )
			.def( pybind11::init<const Vec3f &, const Vec3f &>(), pybind11::arg( "min" ), pybind11::arg( "max" ) )
			.def( pybind11::init<const Vec3f &, const float>(), pybind11::arg( "center" ), pybind11::arg( "radius" ) )
			.def_property_readonly( "min", []( const AABB & p_aabb ) { return p_aabb.getMin(); } )
			.def_property_readonly( "max", []( const AABB & p_aabb ) { return p_aabb.getMax(); } )
			.def_property_readonly( "center", []( const AABB & p_aabb ) { return p_aabb.centroid(); } )
			.def_property_readonly( "radius", &AABB::radius )
			.def_property_readonly( "valid", &AABB::isValid );

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
				"getChain", []( const API::System & p_mol, const Index & p_index ) { return p_mol.getChain( p_index ); }
			)
			.def( "getChain", []( API::System & p_mol, const Index & p_index ) { return p_mol.getChain( p_index ); } )
			.def( "getAtom", []( const API::System & p_mol, const Index p_index ) { return p_mol.getAtom( p_index ); } )
			.def( "getAtom", []( API::System & p_mol, const Index p_index ) { return p_mol.getAtom( p_index ); } )
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
				[]( const API::System & p_mol, const Index & p_index ) { return p_mol.getResidue( p_index ); }
			)
			.def(
				"getResidue", []( API::System & p_mol, const Index & p_index ) { return p_mol.getResidue( p_index ); }
			)

			;

		// Residue
		Helper::declareEnum<VTX::Core::ChemDB::Residue::SYMBOL>( p_apiModule, "RESIDUE_SYMBOL" );

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
		Helper::declareEnum<VTX::Core::ChemDB::Atom::SYMBOL>( p_apiModule, "ATOM_SYMBOL" );
		Helper::declareEnum<VTX::Core::ChemDB::Atom::TYPE>( p_apiModule, "ATOM_TYPE" );

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
