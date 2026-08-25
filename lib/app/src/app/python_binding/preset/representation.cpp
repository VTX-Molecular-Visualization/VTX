#include "app/python_binding/preset/representation.hpp"
#include "app/action/representation.hpp"
#include "app/python_binding/preset/helpers.hpp"
#include "app/python_binding/topology/helpers.hpp"
#include "app/python_binding/topology/types.hpp"
#include <pybind11/native_enum.h>
#include <python_binding/binding/helper.hpp>
#include <renderer/representation.hpp>
#include <util/enum.hpp>

namespace VTX::App::PythonBinding::Preset
{
	namespace
	{
		template<Renderer::E_REPRESENTATION_VALUES VALUE, typename T>
		void _setValue( const RepresentationHandle & p_preset, const T p_value )
		{
			validate( p_preset );
			executeAction<Action::Representation::Change<VALUE, T>>( Entity( p_preset.entity ), T( p_value ) );
		}

		void _apply(
			const RepresentationHandle & p_preset,
			const Topology::System &	 p_system,
			const Topology::SystemItem	 p_item,
			const Topology::RangeList &	 p_ranges
		)
		{
			validate( p_preset );
			Topology::getTopology( p_system.entity );
			executeAction<Action::Representation::AddItem>(
				Entity( p_system.entity ),
				Topology::SystemItem( p_item ),
				Entity( p_preset.entity ),
				Topology::RangeList( p_ranges )
			);
		}

	} // namespace

	void bindRepresentation( Module & p_vtxModule, pybind11::class_<RepresentationHandle> & p_binding )
	{
		VTX::PythonBinding::Helper::declareEnum<Renderer::E_REPRESENTATION_VALUES>(
			p_vtxModule.pyModule(), "REPRESENTATION_VALUE"
		);
		pybind11::native_enum<Renderer::E_SES_COMPUTE_MODE> computeMode(
			p_vtxModule.pyModule(), "SES_COMPUTE_MODE", "enum.IntFlag"
		);
		for ( const Renderer::E_SES_COMPUTE_MODE value : magic_enum::enum_values<Renderer::E_SES_COMPUTE_MODE>() )
		{
			computeMode.value( Util::Enum::enumName( value ).data(), value );
		}
		computeMode.finalize();

		p_binding
			.def_property(
				"hasSphere",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).hasSphere; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::HAS_SPHERE, bool>
			)
			.def_property(
				"radiusSphereFixed",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).radiusSphereFixed; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::RADIUS_SPHERE_FIXED, float>
			)
			.def_property(
				"radiusSphereAdd",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).radiusSphereAdd; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::RADIUS_SPHERE_ADD, float>
			)
			.def_property(
				"isRadiusSphereFixed",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).isRadiusSphereFixed; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::IS_SPHERE_RADIUS_FIXED, bool>
			)
			.def_property(
				"hasCylinder",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).hasCylinder; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::HAS_CYLINDER, bool>
			)
			.def_property(
				"radiusCylinder",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).radiusCylinder; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::RADIUS_CYLINDER, float>
			)
			.def_property(
				"cylinderColorBlending",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).cylinderColorBlending; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::CYLINDER_COLOR_BLENDING, bool>
			)
			.def_property(
				"hasRibbon",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).hasRibbon; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::HAS_RIBBON, bool>
			)
			.def_property(
				"ribbonColorBlending",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).ribbonColorBlending != 0; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::RIBBON_COLOR_BLENDING, bool>
			)
			.def_property(
				"hasSes",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).hasSes; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::HAS_SES, bool>
			)
			.def_property(
				"sesComputeMode",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).sesComputeMode; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::SES_COMPUTE_MODE, Renderer::E_SES_COMPUTE_MODE>
			)
			.def_property(
				"sesProbeRadius",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).sesProbeRadius; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::SES_PROBE_RADIUS, float>
			)
			.def(
				"apply",
				&_apply,
				"Apply this representation preset to system items.",
				pybind11::arg( "system" ),
				pybind11::arg( "item" )	  = Topology::SystemItem::SYSTEM,
				pybind11::arg( "ranges" ) = Topology::RangeList()
			);
	}
} // namespace VTX::App::PythonBinding::Preset
