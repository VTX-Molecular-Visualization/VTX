#include "app/application/representation/data/ball_and_stick.hpp"
#include "app/application/representation/enum_representation.hpp"
#include "app/application/setting.hpp"

namespace VTX::App::Application::Representation::Data
{
	BallAndStick::BallAndStick() : RepresentationData( REPRESENTATION_ENUM::BALL_AND_STICK )
	{
		_sphereData				   = new Primitive::Sphere();
		_sphereData->radiusFixed   = VTX::App::Application::Setting::ATOMS_RADIUS_DEFAULT;
		_sphereData->radiusAdd	   = 0;
		_sphereData->isRadiusFixed = true;

		_cylinderData					 = new Primitive::Cylinder();
		_cylinderData->radius			 = VTX::App::Application::Setting::BONDS_RADIUS_DEFAULT;
		_cylinderData->colorBlendingMode = VTX::App::Application::Setting::BONDS_COLOR_BLENDING_MODE_DEFAULT;
	}

	void BallAndStick::setSphereRadius( float p_radius )
	{
		_sphereData->radiusFixed = p_radius;

		if ( _sphereData->radiusFixed < _cylinderData->radius )
			_cylinderData->radius = p_radius;

		notifyRepresentationDataChange();
	}
	void BallAndStick::setCylinderRadius( float p_radius )
	{
		_cylinderData->radius = p_radius;

		if ( _cylinderData->radius > _sphereData->radiusFixed )
			_sphereData->radiusFixed = p_radius;

		notifyRepresentationDataChange();
	}
} // namespace VTX::App::Application::Representation::Data
