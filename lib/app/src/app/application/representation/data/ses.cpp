#include "app/application/representation/data/ses.hpp"
#include "app/application/representation/enum_representation.hpp"
#include "app/old_app/setting.hpp"

namespace VTX::App::Application::Representation::Data
{
	Ses::Ses() : RepresentationData( REPRESENTATION_ENUM::SES )
	{
		_sesData			 = new Primitive::SES();
		_sesData->resolution = Setting::SES_RESOLUTION_DEFAULT;
	}

} // namespace VTX::App::Application::Representation::Data
