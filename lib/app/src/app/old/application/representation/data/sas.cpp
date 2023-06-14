#include "app/old/application/representation/data/sas.hpp"
#include "app/old/application/representation/enum_representation.hpp"
#include "app/old/application/setting.hpp"

namespace VTX::App::Old::Application::Representation::Data
{
	Sas::Sas() : RepresentationData( REPRESENTATION_ENUM::SAS )
	{
		_sphereData				   = new Primitive::Sphere();
		_sphereData->isRadiusFixed = false;
		_sphereData->radiusAdd	   = 1.4f;
	}
} // namespace VTX::App::Old::Application::Representation::Data
