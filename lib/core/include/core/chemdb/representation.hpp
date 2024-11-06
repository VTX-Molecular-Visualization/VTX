#ifndef __VTX_CORE_CHEMDB_REPRESENTATION__
#define __VTX_CORE_CHEMDB_REPRESENTATION__

namespace VTX::Core::ChemDB::Representation
{
	// TODO: move to Renderer module?
	enum struct E_REPRESENTATION_SETTINGS
	{
		HAS_SPHERE,
		RADIUS_SPHERE_FIXED,
		RADIUS_SPHERE_ADD,
		RADIUS_FIXED,

		HAS_CYLINDER,
		RADIUS_CYLINDER,
		CYLINDER_COLOR_BLENDING,

		HAS_RIBBON,
		RIBBON_COLOR_BLENDING,

		HAS_SES
	};

} // namespace VTX::Core::ChemDB::Representation

#endif
