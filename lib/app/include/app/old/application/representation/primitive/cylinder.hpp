#ifndef __VTX_APP_OLD_INTERNAL_REPRESENTATION_PRIMITIVE_CYLINDER__
#define __VTX_APP_OLD_INTERNAL_REPRESENTATION_PRIMITIVE_CYLINDER__

#include <core/chemdb/color.hpp>

namespace VTX::App::Old::Application::Representation::Primitive
{
	struct Cylinder
	{
	  public:
		Cylinder();

		float										  radius;
		VTX::Core::ChemDB::Color::COLOR_BLENDING_MODE colorBlendingMode;
	};

} // namespace VTX::App::Old::Application::Representation::Primitive
#endif
