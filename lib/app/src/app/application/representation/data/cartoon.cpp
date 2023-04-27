#include "app/application/representation/data/cartoon.hpp"
#include "app/application/representation/enum_representation.hpp"
#include "app/old_app/setting.hpp"

namespace VTX::App::Application::Representation::Data
{
	Cartoon::Cartoon() : RepresentationData( REPRESENTATION_ENUM::CARTOON )
	{
		_ribbonData					   = new Primitive::Ribbon();
		_ribbonData->colorBlendingMode = Setting::SS_COLOR_BLENDING_MODE_DEFAULT;
	}

} // namespace VTX::App::Application::Representation::Data
