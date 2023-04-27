#include "app/application/representation/data/stick.hpp"
#include "app/application/representation/enum_representation.hpp"
#include "app/old_app/setting.hpp"

namespace VTX::App::Application::Representation::Data
{
	Stick::Stick() : RepresentationData( REPRESENTATION_ENUM::STICK )
	{
		_sphereData				   = new Primitive::Sphere();
		_sphereData->radiusFixed   = Setting::BONDS_RADIUS_DEFAULT;
		_sphereData->radiusAdd	   = 0;
		_sphereData->isRadiusFixed = true;

		_cylinderData					 = new Primitive::Cylinder();
		_cylinderData->radius			 = Setting::BONDS_RADIUS_DEFAULT;
		_cylinderData->colorBlendingMode = Setting::BONDS_COLOR_BLENDING_MODE_DEFAULT;
	}

	void Stick::setSphereRadius( const float p_radius )
	{
		_sphereData->radiusFixed = p_radius;
		_cylinderData->radius	 = p_radius;
		notifyRepresentationDataChange();
	}
	void Stick::setCylinderRadius( const float p_radius )
	{
		_sphereData->radiusFixed = p_radius;
		_cylinderData->radius	 = p_radius;
		notifyRepresentationDataChange();
	}
} // namespace VTX::App::Application::Representation::Data
