#include "app/old/application/representation/data/ses.hpp"
#include "app/old/application/representation/enum_representation.hpp"
#include "app/old/application/setting.hpp"

namespace VTX::App::Old::Application::Representation::Data
{
	Ses::Ses() : RepresentationData( REPRESENTATION_ENUM::SES )
	{
		_sesData			 = new Primitive::SES();
		_sesData->resolution = VTX::App::Old::Application::Setting::SES_RESOLUTION_DEFAULT;
	}

} // namespace VTX::App::Old::Application::Representation::Data
