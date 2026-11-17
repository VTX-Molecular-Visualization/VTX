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
		pybind11::native_enum<Renderer::E_SES_COMPUTE_MODE> computeMode(
			p_vtxModule.pyModule(),
			"SES_COMPUTE_MODE",
			"enum.IntFlag",
			"Residue categories included in the surface. Values can be combined with |."
		);
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
				&_setValue<Renderer::E_REPRESENTATION_VALUES::HAS_SPHERE, bool>,
				"Whether atoms are drawn as spheres."
			)
			.def_property(
				"radiusSphereFixed",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).radiusSphereFixed; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::RADIUS_SPHERE_FIXED, float>,
				"Sphere radius in angstroms when the radius is fixed."
			)
			.def_property(
				"radiusSphereAdd",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).radiusSphereAdd; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::RADIUS_SPHERE_ADD, float>,
				"Radius added to the van der Waals radius when the radius is not fixed."
			)
			.def_property(
				"isRadiusSphereFixed",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).isRadiusSphereFixed; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::IS_SPHERE_RADIUS_FIXED, bool>,
				"Whether all spheres use the fixed radius."
			)
			.def_property(
				"hasCylinder",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).hasCylinder; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::HAS_CYLINDER, bool>,
				"Whether bonds are drawn as cylinders."
			)
			.def_property(
				"radiusCylinder",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).radiusCylinder; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::RADIUS_CYLINDER, float>,
				"Cylinder radius in angstroms."
			)
			.def_property(
				"cylinderColorBlending",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).cylinderColorBlending; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::CYLINDER_COLOR_BLENDING, bool>,
				"Whether cylinder colors blend between atoms."
			)
			.def_property(
				"hasRibbon",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).hasRibbon; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::HAS_RIBBON, bool>,
				"Whether secondary structure is drawn as ribbons."
			)
			.def_property(
				"ribbonColorBlending",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).ribbonColorBlending != 0; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::RIBBON_COLOR_BLENDING, bool>,
				"Whether ribbon colors blend between residues."
			)
			.def_property(
				"hasSes",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).hasSes; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::HAS_SES, bool>,
				"Whether the solvent excluded surface is drawn."
			)
			.def_property(
				"sesComputeMode",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).sesComputeMode; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::SES_COMPUTE_MODE, Renderer::E_SES_COMPUTE_MODE>,
				"Residue categories included in the surface (SES_COMPUTE_MODE flags)."
			)
			.def_property(
				"sesProbeRadius",
				[]( const RepresentationHandle & p_preset ) { return getData( p_preset ).sesProbeRadius; },
				&_setValue<Renderer::E_REPRESENTATION_VALUES::SES_PROBE_RADIUS, float>,
				"Surface probe radius in angstroms."
			);
	}
} // namespace VTX::App::PythonBinding::Preset
