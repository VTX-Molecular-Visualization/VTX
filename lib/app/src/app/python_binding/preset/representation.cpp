#include "app/python_binding/preset/representation.hpp"
#include "app/action/representation.hpp"
#include "app/python_binding/preset/helpers.hpp"
#include <pybind11/native_enum.h>
#include <python_binding/binding/helper.hpp>
#include <renderer/representation.hpp>
#include <string>
#include <util/enum.hpp>

namespace VTX::App::PythonBinding::Preset
{
	namespace
	{
		template<Renderer::E_REPRESENTATION_VALUES VALUE, typename T>
		void _setValue( const RepresentationHandle & p_preset, const T p_value )
		{ executePresetAction<Action::Representation::Change<VALUE, T>>( p_preset, T( p_value ) ); }

	} // namespace

	void bindRepresentation( Module & p_vtxModule, pybind11::class_<RepresentationHandle> & p_binding )
	{
		VTX::PythonBinding::Helper::declareEnum<Renderer::E_REPRESENTATION_VALUES>( p_vtxModule.pyModule(),
																					"REPRESENTATION_VALUE" );
		pybind11::native_enum<Renderer::E_SES_COMPUTE_MODE> computeMode(
			p_vtxModule.pyModule(), "SES_COMPUTE_MODE", "enum.IntFlag" );
		for ( const Renderer::E_SES_COMPUTE_MODE value : magic_enum::enum_values<Renderer::E_SES_COMPUTE_MODE>() )
		{
			const std::string enumName( Util::Enum::enumName( value ) );
			computeMode.value( enumName.c_str(), value );
		}
		computeMode.finalize();

		p_binding
			.def_property(
				"hasSphere",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).hasSphere; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::HAS_SPHERE, bool> )
			.def_property(
				"radiusSphereFixed",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).radiusSphereFixed; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::RADIUS_SPHERE_FIXED, float> )
			.def_property(
				"radiusSphereAdd",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).radiusSphereAdd; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::RADIUS_SPHERE_ADD, float> )
			.def_property(
				"isRadiusSphereFixed",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).isRadiusSphereFixed; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::IS_SPHERE_RADIUS_FIXED, bool> )
			.def_property(
				"hasCylinder",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).hasCylinder; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::HAS_CYLINDER, bool> )
			.def_property(
				"radiusCylinder",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).radiusCylinder; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::RADIUS_CYLINDER, float> )
			.def_property(
				"cylinderColorBlending",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).cylinderColorBlending; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::CYLINDER_COLOR_BLENDING, bool> )
			.def_property(
				"hasRibbon",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).hasRibbon; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::HAS_RIBBON, bool> )
			.def_property(
				"ribbonColorBlending",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).ribbonColorBlending != 0; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::RIBBON_COLOR_BLENDING, bool> )
			.def_property(
				"hasSes",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).hasSes; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::HAS_SES, bool> )
			.def_property(
				"sesComputeMode",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).sesComputeMode; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::SES_COMPUTE_MODE, Renderer::E_SES_COMPUTE_MODE> )
			.def_property(
				"sesProbeRadius",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).sesProbeRadius; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::SES_PROBE_RADIUS, float> );
	}
} // namespace VTX::App::PythonBinding::Preset
