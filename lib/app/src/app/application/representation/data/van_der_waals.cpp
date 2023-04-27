#include "app/application/representation/data/van_der_waals.hpp"
#include "app/application/representation/enum_representation.hpp"
#include "app/old_app/setting.hpp"

namespace VTX::App::Application::Representation::Data
{
	VanDerWaals::VanDerWaals() : RepresentationData( REPRESENTATION_ENUM::VAN_DER_WAALS )
	{
		_sphereData				   = new Primitive::Sphere();
		_sphereData->radiusAdd	   = 0;
		_sphereData->isRadiusFixed = false;
	}

	void VanDerWaals::setSphereRadius( const float p_radius )
	{
		_sphereData->radiusAdd = p_radius;
		notifyRepresentationDataChange();
	}

} // namespace VTX::App::Application::Representation::Data
