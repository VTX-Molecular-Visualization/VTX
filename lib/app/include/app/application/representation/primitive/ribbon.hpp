#ifndef __VTX_APP_INTERNAL_REPRESENTATION_PRIMITIVE_RIBBON__
#define __VTX_APP_INTERNAL_REPRESENTATION_PRIMITIVE_RIBBON__

#include <core/chemdb/color.hpp>

namespace VTX::App::Application::Representation::Primitive
{
	struct Ribbon
	{
	  public:
		Ribbon();

		VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE colorMode;
		VTX::Core::ChemDB::Color::COLOR_BLENDING_MODE			 colorBlendingMode;
	};

} // namespace VTX::App::Application::Representation::Primitive
#endif
