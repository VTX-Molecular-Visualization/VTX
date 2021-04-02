#include "all_representation_data.hpp"
#include "setting.hpp"
#include "vtx_app.hpp"

namespace VTX::Model::Representation
{
	RepresentationData_VanDerWaals::RepresentationData_VanDerWaals() :
		RepresentationData( Generic::REPRESENTATION::VAN_DER_WAALS )
	{
		_dataTargeted = VTX::Representation::FlagDataTargeted::ATOM;

		_sphereData					= new SphereData();
		_sphereData->_radiusAdd		= 0;
		_sphereData->_isRadiusFixed = false;
	}

	RepresentationData_BallsAndSticks::RepresentationData_BallsAndSticks() :
		RepresentationData( Generic::REPRESENTATION::BALL_AND_STICK )
	{
		_dataTargeted = VTX::Representation::FlagDataTargeted( VTX::Representation::FlagDataTargeted::ATOM
															   | VTX::Representation::FlagDataTargeted::BOND );

		_sphereData					= new SphereData();
		_sphereData->_radiusFixed	= Util::Math::max( VTX_SETTING().bondsRadius, VTX_SETTING().atomsRadius );
		_sphereData->_radiusAdd		= 0;
		_sphereData->_isRadiusFixed = true;

		_cylinderData		   = new CylinderData();
		_cylinderData->_radius = VTX_SETTING().bondsRadius;
	}

	RepresentationData_Sticks::RepresentationData_Sticks() : RepresentationData( Generic::REPRESENTATION::STICK )
	{
		_dataTargeted = VTX::Representation::FlagDataTargeted( VTX::Representation::FlagDataTargeted::ATOM
															   | VTX::Representation::FlagDataTargeted::BOND );

		_sphereData					= new SphereData();
		_sphereData->_radiusFixed	= VTX_SETTING().bondsRadius;
		_sphereData->_radiusAdd		= 0;
		_sphereData->_isRadiusFixed = true;

		_cylinderData		   = new CylinderData();
		_cylinderData->_radius = VTX_SETTING().bondsRadius;
	}

	RepresentationData_Trace::RepresentationData_Trace() : RepresentationData( Generic::REPRESENTATION::TRACE )
	{
		_dataTargeted = VTX::Representation::FlagDataTargeted( VTX::Representation::FlagDataTargeted::ATOM
															   | VTX::Representation::FlagDataTargeted::BOND );

		_sphereData					= new SphereData();
		_sphereData->_radiusFixed	= VTX_SETTING().atomsRadius;
		_sphereData->_radiusAdd		= -0.5f;
		_sphereData->_isRadiusFixed = true;

		_cylinderData		   = new CylinderData();
		_cylinderData->_radius = VTX_SETTING().bondsRadius;
	}

	RepresentationData_Sas::RepresentationData_Sas() : RepresentationData( Generic::REPRESENTATION::SAS )
	{
		_dataTargeted = VTX::Representation::FlagDataTargeted( VTX::Representation::FlagDataTargeted::ATOM
															   | VTX::Representation::FlagDataTargeted::BOND );

		_sphereData					= new SphereData();
		_sphereData->_isRadiusFixed = false;
		_sphereData->_radiusAdd		= 1.4f;
	}

	RepresentationData_Cartoon::RepresentationData_Cartoon() : RepresentationData( Generic::REPRESENTATION::CARTOON )
	{
		_dataTargeted = VTX::Representation::FlagDataTargeted::RIBBON;

		_ribbonData = new RibbonData();
	}

	RepresentationData_BallStickAndCartoon::RepresentationData_BallStickAndCartoon() :
		RepresentationData( Generic::REPRESENTATION::BALL_AND_STICK_AND_CARTOON )
	{
		_dataTargeted = VTX::Representation::FlagDataTargeted( VTX::Representation::FlagDataTargeted::ATOM
															   | VTX::Representation::FlagDataTargeted::BOND
															   | VTX::Representation::FlagDataTargeted::RIBBON );

		_sphereData					= new SphereData();
		_sphereData->_radiusFixed	= Util::Math::max( VTX_SETTING().bondsRadius, VTX_SETTING().atomsRadius );
		_sphereData->_radiusAdd		= 0;
		_sphereData->_isRadiusFixed = true;

		_cylinderData		   = new CylinderData();
		_cylinderData->_radius = VTX_SETTING().bondsRadius;

		_ribbonData = new RibbonData();
	}

	RepresentationData_StickAndCartoon::RepresentationData_StickAndCartoon() :
		RepresentationData( Generic::REPRESENTATION::STICK_AND_CARTOON )
	{
		_dataTargeted = VTX::Representation::FlagDataTargeted( VTX::Representation::FlagDataTargeted::ATOM
															   | VTX::Representation::FlagDataTargeted::BOND
															   | VTX::Representation::FlagDataTargeted::RIBBON );

		_sphereData					= new SphereData();
		_sphereData->_radiusFixed	= VTX_SETTING().bondsRadius;
		_sphereData->_radiusAdd		= 0;
		_sphereData->_isRadiusFixed = true;

		_cylinderData		   = new CylinderData();
		_cylinderData->_radius = VTX_SETTING().bondsRadius;

		_ribbonData = new RibbonData();
	}

} // namespace VTX::Model::Representation
