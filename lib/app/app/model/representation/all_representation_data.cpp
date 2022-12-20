#include "all_representation_data.hpp"
#include "default_values.hpp"

namespace VTX::Model::Representation
{
	RepresentationData_VanDerWaals::RepresentationData_VanDerWaals() :
		RepresentationData( Generic::REPRESENTATION::VAN_DER_WAALS )
	{
		_sphereData				   = new SphereData();
		_sphereData->radiusAdd	   = 0;
		_sphereData->isRadiusFixed = false;
	}

	RepresentationData_BallsAndSticks::RepresentationData_BallsAndSticks() :
		RepresentationData( Generic::REPRESENTATION::BALL_AND_STICK )
	{
		_sphereData				   = new SphereData();
		_sphereData->radiusFixed   = Setting::ATOMS_RADIUS_DEFAULT;
		_sphereData->radiusAdd	   = 0;
		_sphereData->isRadiusFixed = true;

		_cylinderData					 = new CylinderData();
		_cylinderData->radius			 = Setting::BONDS_RADIUS_DEFAULT;
		_cylinderData->colorBlendingMode = Setting::BONDS_COLOR_BLENDING_MODE_DEFAULT;
	}

	RepresentationData_Sticks::RepresentationData_Sticks() : RepresentationData( Generic::REPRESENTATION::STICK )
	{
		_sphereData				   = new SphereData();
		_sphereData->radiusFixed   = Setting::BONDS_RADIUS_DEFAULT;
		_sphereData->radiusAdd	   = 0;
		_sphereData->isRadiusFixed = true;

		_cylinderData					 = new CylinderData();
		_cylinderData->radius			 = Setting::BONDS_RADIUS_DEFAULT;
		_cylinderData->colorBlendingMode = Setting::BONDS_COLOR_BLENDING_MODE_DEFAULT;
	}

	// !V0.1
	// RepresentationData_Trace::RepresentationData_Trace() : RepresentationData( Generic::REPRESENTATION::TRACE )
	//{
	//	_sphereData					= new SphereData();
	//	_sphereData->_radiusFixed	= Setting::BONDS_RADIUS_DEFAULT;
	//	_sphereData->_radiusAdd		= -0.5f;
	//	_sphereData->_isRadiusFixed = true;

	//	_cylinderData		   = new CylinderData();
	//	_cylinderData->_radius = Setting::BONDS_RADIUS_DEFAULT;
	//}

	RepresentationData_Sas::RepresentationData_Sas() : RepresentationData( Generic::REPRESENTATION::SAS )
	{
		_sphereData				   = new SphereData();
		_sphereData->isRadiusFixed = false;
		_sphereData->radiusAdd	   = 1.4f;
	}

	RepresentationData_Cartoon::RepresentationData_Cartoon() : RepresentationData( Generic::REPRESENTATION::CARTOON )
	{
		_ribbonData					   = new RibbonData();
		_ribbonData->colorBlendingMode = Setting::SS_COLOR_BLENDING_MODE_DEFAULT;
	}

	RepresentationData_BallStickAndCartoon::RepresentationData_BallStickAndCartoon() :
		RepresentationData( Generic::REPRESENTATION::BALL_AND_STICK_AND_CARTOON )
	{
		_sphereData				   = new SphereData();
		_sphereData->radiusFixed   = Setting::ATOMS_RADIUS_DEFAULT;
		_sphereData->radiusAdd	   = 0;
		_sphereData->isRadiusFixed = true;

		_cylinderData					 = new CylinderData();
		_cylinderData->radius			 = Setting::BONDS_RADIUS_DEFAULT;
		_cylinderData->colorBlendingMode = Setting::BONDS_COLOR_BLENDING_MODE_DEFAULT;

		_ribbonData					   = new RibbonData();
		_ribbonData->colorBlendingMode = Setting::SS_COLOR_BLENDING_MODE_DEFAULT;
	}

	RepresentationData_StickAndCartoon::RepresentationData_StickAndCartoon() :
		RepresentationData( Generic::REPRESENTATION::STICK_AND_CARTOON )
	{
		_sphereData				   = new SphereData();
		_sphereData->radiusFixed   = Setting::BONDS_RADIUS_DEFAULT;
		_sphereData->radiusAdd	   = 0;
		_sphereData->isRadiusFixed = true;

		_cylinderData					 = new CylinderData();
		_cylinderData->radius			 = Setting::BONDS_RADIUS_DEFAULT;
		_cylinderData->colorBlendingMode = Setting::BONDS_COLOR_BLENDING_MODE_DEFAULT;

		_ribbonData					   = new RibbonData();
		_ribbonData->colorBlendingMode = Setting::SS_COLOR_BLENDING_MODE_DEFAULT;
	}

	RepresentationData_Ses::RepresentationData_Ses() : RepresentationData( Generic::REPRESENTATION::SES )
	{
		_sesData			 = new SESData();
		_sesData->resolution = Setting::SES_RESOLUTION_DEFAULT;
	}

} // namespace VTX::Model::Representation
