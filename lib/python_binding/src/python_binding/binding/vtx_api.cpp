#include "python_binding/binding/vtx_api.hpp"
#include "python_binding/log_redirection.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <util/color/rgba.hpp>
#include <util/math.hpp>
#include <util/math/aabb.hpp>
#include <util/math/range.hpp>
#include <util/math/range_list.hpp>
#include <util/types.hpp>

namespace VTX::PythonBinding::Binding
{

	void applyVtxApiBinding( pybind11::module_ & p_apiModule )
	{
		p_apiModule.doc()
			= "VTX Python module. Will contain every functions and classes provided by VTX to interact with the "
			  "renderer and its underlying data. Please use the function help([Anything]) on any element of the module "
			  "to get help message."; // optional module docstring

		// Class to redirect Python prints
		pybind11::class_<LogRedirection>( p_apiModule, "LogRedirection", pybind11::module_local() )
			.def_static( "write", &LogRedirection::write )
			.def_static( "flush", &LogRedirection::flush );

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
			.def( pybind11::init( []() { return Quatf( 1.f, 0.f, 0.f, 0.f ); } ) )
			.def(
				pybind11::init( []( const float p_w, const float p_x, const float p_y, const float p_z )
								{ return Quatf( p_w, p_x, p_y, p_z ); } ),
				pybind11::arg( "w" ),
				pybind11::arg( "x" ),
				pybind11::arg( "y" ),
				pybind11::arg( "z" )
			)
			.def_static(
				"fromEuler",
				[]( const float p_x, const float p_y, const float p_z )
				{ return Quatf( Util::Math::radians( Vec3f( p_x, p_y, p_z ) ) ); },
				"Build a rotation from Euler angles in degrees (pitch, yaw, roll).",
				pybind11::arg( "x" ),
				pybind11::arg( "y" ),
				pybind11::arg( "z" )
			)
			.def_property_readonly(
				"euler",
				[]( const Quatf & p_quat )
				{ return Vec3f( Util::Math::degrees( Util::Math::eulerAngles( p_quat ) ) ); },
				"Euler angles in degrees (pitch, yaw, roll)."
			)
			.def_readwrite( "x", &Quatf::x )
			.def_readwrite( "y", &Quatf::y )
			.def_readwrite( "z", &Quatf::z )
			.def_readwrite( "w", &Quatf::w );

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

		using Range = Util::Math::Range<Index>;
		pybind11::class_<Range>( p_apiModule, "Range", pybind11::module_local() )
			.def( pybind11::init<>() )
			.def( pybind11::init<const Index>(), pybind11::arg( "value" ) )
			.def( pybind11::init<const Index, const Index>(), pybind11::arg( "first" ), pybind11::arg( "last" ) )
			.def_property_readonly( "first", &Range::getFirst )
			.def_property_readonly( "last", &Range::getLast )
			.def_property_readonly( "count", &Range::getCount );

		using RangeList = Util::Math::RangeList<Index>;
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
		pybind11::implicitly_convertible<Range, RangeList>();
		pybind11::implicitly_convertible<pybind11::list, RangeList>();
		pybind11::implicitly_convertible<pybind11::tuple, RangeList>();

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
	}
} // namespace VTX::PythonBinding::Binding
