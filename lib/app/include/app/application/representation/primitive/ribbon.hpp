#ifndef __VTX_APP_INTERNAL_REPRESENTATION_PRIMITIVE_RIBBON__
#define __VTX_APP_INTERNAL_REPRESENTATION_PRIMITIVE_RIBBON__

#include "app/internal/chemdb/color.hpp"

namespace VTX::App::Application::Representation::Primitive
{
	struct Ribbon
	{
	  public:
		Ribbon();

		App::Internal::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE colorMode;
		App::Internal::ChemDB::Color::COLOR_BLENDING_MODE			 colorBlendingMode;
	};

} // namespace VTX::App::Application::Representation::Primitive
#endif
