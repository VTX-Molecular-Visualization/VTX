#include "app/library/preset/representation.hpp"

namespace VTX::App::Library::Preset
{
	Representation::Representation()
	{
		// Set default values.
		_data.hasSphere				= HAS_SPHERE_DEFAULT;
		_data.radiusSphereFixed		= RADIUS_SPHERE_FIXED_DEFAULT;
		_data.radiusSphereAdd		= RADIUS_SPHERE_ADD_DEFAULT;
		_data.isRadiusSphereFixed	= IS_SPHERE_RADIUS_FIXED_DEFAULT;
		_data.hasCylinder			= HAS_CYLINDER_DEFAULT;
		_data.radiusCylinder		= RADIUS_CYLINDER_DEFAULT;
		_data.cylinderColorBlending = CYLINDER_COLOR_BLENDING_DEFAULT;
		_data.hasRibbon				= HAS_RIBBON_DEFAULT;
		_data.ribbonColorBlending	= RIBBON_COLOR_BLENDING_DEFAULT;
		_data.hasSes				= HAS_SES_DEFAULT;
		_data.sesProbeRadius		= SES_PROBE_RADIUS_DEFAULT;
	}

	void Representation::save() {}
	void Representation::load() {}
} // namespace VTX::App::Library::Preset
