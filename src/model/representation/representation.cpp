#include "representation.hpp"
#include "representation_enum.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Model
	{
		namespace Representation
		{
			Representation_VanDerWaals::Representation_VanDerWaals() : BaseRepresentation( ID::Model::MODEL_REPRESENTATION_BALLSANDSTICKS )
			{
				_representationType = Generic::REPRESENTATION::VAN_DER_WAALS;

				_sphereData					= new SphereData();
				_sphereData->_radiusAdd		= 0;
				_sphereData->_isRadiusFixed = false;
			}
			Representation_BallsAndSticks::Representation_BallsAndSticks() : BaseRepresentation( ID::Model::MODEL_REPRESENTATION_BALLSANDSTICKS )
			{
				_representationType = Generic::REPRESENTATION::BALL_AND_STICK;

				_sphereData					= new SphereData();
				_sphereData->_radiusFixed	= Util::Math::max( VTX_SETTING().bondsRadius, VTX_SETTING().atomsRadius );
				_sphereData->_radiusAdd		= 0;
				_sphereData->_isRadiusFixed = true;

				_cylinderData				= new CylinderData();
				_cylinderData->_radiusFixed = VTX_SETTING().bondsRadius;
			}

			Representation_Sticks::Representation_Sticks() : BaseRepresentation( ID::Model::MODEL_REPRESENTATION_STICKS )
			{
				_representationType = Generic::REPRESENTATION::STICK;

				_sphereData					= new SphereData();
				_sphereData->_radiusFixed	= VTX_SETTING().bondsRadius;
				_sphereData->_radiusAdd		= 0;
				_sphereData->_isRadiusFixed = true;

				_cylinderData				= new CylinderData();
				_cylinderData->_radiusFixed = VTX_SETTING().bondsRadius;
			}

			Representation_Trace::Representation_Trace() : BaseRepresentation( ID::Model::MODEL_REPRESENTATION_STICKS )
			{
				_representationType = Generic::REPRESENTATION::TRACE;

				_sphereData					= new SphereData();
				_sphereData->_radiusFixed	= VTX_SETTING().atomsRadius;
				_sphereData->_radiusAdd		= -0.5f;
				_sphereData->_isRadiusFixed = true;
			}

			Representation_Sas::Representation_Sas() : BaseRepresentation( ID::Model::MODEL_REPRESENTATION_STICKS )
			{
				_representationType = Generic::REPRESENTATION::SAS;

				_sphereData					= new SphereData();
				_sphereData->_isRadiusFixed = false;
				_sphereData->_radiusAdd		= 1.4f;
			}
		} // namespace Representation
	}	  // namespace Model
} // namespace VTX
