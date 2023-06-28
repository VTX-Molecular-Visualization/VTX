#include "app/old/application/representation/data/cartoon.hpp"
#include "app/old/application/representation/enum_representation.hpp"
#include "app/old/application/setting.hpp"

namespace VTX::App::Old::Application::Representation::Data
{
	Cartoon::Cartoon() : RepresentationData( REPRESENTATION_ENUM::CARTOON )
	{
		_ribbonData					   = new Primitive::Ribbon();
		_ribbonData->colorBlendingMode = VTX::App::Old::Application::Setting::SS_COLOR_BLENDING_MODE_DEFAULT;
	}

} // namespace VTX::App::Old::Application::Representation::Data
