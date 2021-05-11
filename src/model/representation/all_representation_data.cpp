#include "all_representation_data.hpp"
#include "setting.hpp"
#include "vtx_app.hpp"

namespace VTX::Model::Representation
{
	RepresentationData_VanDerWaals::RepresentationData_VanDerWaals() :
		RepresentationData( Generic::REPRESENTATION::VAN_DER_WAALS )
	{
		_sphereData					= new SphereData();
		_sphereData->_radiusAdd		= 0;
		_sphereData->_isRadiusFixed = false;
	}

	RepresentationData_BallsAndSticks::RepresentationData_BallsAndSticks() :
		RepresentationData( Generic::REPRESENTATION::BALL_AND_STICK )
	{
		_sphereData					= new SphereData();
		_sphereData->_radiusFixed	= Setting::ATOMS_RADIUS_DEFAULT;
		_sphereData->_radiusAdd		= 0;
		_sphereData->_isRadiusFixed = true;

		_cylinderData		   = new CylinderData();
		_cylinderData->_radius = Setting::BONDS_RADIUS_DEFAULT;
	}

	RepresentationData_Sticks::RepresentationData_Sticks() : RepresentationData( Generic::REPRESENTATION::STICK )
	{
		_sphereData					= new SphereData();
		_sphereData->_radiusFixed	= Setting::BONDS_RADIUS_DEFAULT;
		_sphereData->_radiusAdd		= 0;
		_sphereData->_isRadiusFixed = true;

		_cylinderData		   = new CylinderData();
		_cylinderData->_radius = Setting::BONDS_RADIUS_DEFAULT;
	}

	RepresentationData_Trace::RepresentationData_Trace() : RepresentationData( Generic::REPRESENTATION::TRACE )
	{
		_sphereData					= new SphereData();
		_sphereData->_radiusFixed	= Setting::BONDS_RADIUS_DEFAULT;
		_sphereData->_radiusAdd		= -0.5f;
		_sphereData->_isRadiusFixed = true;

		_cylinderData		   = new CylinderData();
		_cylinderData->_radius = Setting::BONDS_RADIUS_DEFAULT;
	}

	RepresentationData_Sas::RepresentationData_Sas() : RepresentationData( Generic::REPRESENTATION::SAS )
	{
		_sphereData					= new SphereData();
		_sphereData->_isRadiusFixed = false;
		_sphereData->_radiusAdd		= 1.4f;
	}

	RepresentationData_Cartoon::RepresentationData_Cartoon() : RepresentationData( Generic::REPRESENTATION::CARTOON )
	{
		_ribbonData = new RibbonData();
	}

	RepresentationData_BallStickAndCartoon::RepresentationData_BallStickAndCartoon() :
		RepresentationData( Generic::REPRESENTATION::BALL_AND_STICK_AND_CARTOON )
	{
		_sphereData					= new SphereData();
		_sphereData->_radiusFixed	= Setting::ATOMS_RADIUS_DEFAULT;
		_sphereData->_radiusAdd		= 0;
		_sphereData->_isRadiusFixed = true;

		_cylinderData		   = new CylinderData();
		_cylinderData->_radius = Setting::BONDS_RADIUS_DEFAULT;

		_ribbonData = new RibbonData();
	}

	RepresentationData_StickAndCartoon::RepresentationData_StickAndCartoon() :
		RepresentationData( Generic::REPRESENTATION::STICK_AND_CARTOON )
	{
		_sphereData					= new SphereData();
		_sphereData->_radiusFixed	= Setting::BONDS_RADIUS_DEFAULT;
		_sphereData->_radiusAdd		= 0;
		_sphereData->_isRadiusFixed = true;

		_cylinderData		   = new CylinderData();
		_cylinderData->_radius = Setting::BONDS_RADIUS_DEFAULT;

		_ribbonData = new RibbonData();
	}

} // namespace VTX::Model::Representation
