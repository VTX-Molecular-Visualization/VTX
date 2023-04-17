#ifndef __VTX_MODEL_ALL_REPRESENTATION_DATA__
#define __VTX_MODEL_ALL_REPRESENTATION_DATA__

#include "representation_data.hpp"

namespace VTX::Model::Representation
{
	class RepresentationData_BallsAndSticks : public RepresentationData
	{
	  public:
		RepresentationData_BallsAndSticks();

		void setSphereRadius( float p_radius ) override
		{
			_sphereData->radiusFixed = p_radius;

			if ( _sphereData->radiusFixed < _cylinderData->radius )
				_cylinderData->radius = p_radius;

			notifyRepresentationDataChange();
		}
		void setCylinderRadius( float p_radius ) override
		{
			_cylinderData->radius = p_radius;

			if ( _cylinderData->radius > _sphereData->radiusFixed )
				_sphereData->radiusFixed = p_radius;

			notifyRepresentationDataChange();
		}
	};

	class RepresentationData_BallStickAndCartoon : public RepresentationData
	{
	  public:
		RepresentationData_BallStickAndCartoon();

		void setSphereRadius( float p_radius ) override
		{
			_sphereData->radiusFixed = p_radius;

			if ( _sphereData->radiusFixed < _cylinderData->radius )
				_cylinderData->radius = p_radius;

			notifyRepresentationDataChange();
		}
		void setCylinderRadius( float p_radius ) override
		{
			_cylinderData->radius = p_radius;

			if ( _cylinderData->radius > _sphereData->radiusFixed )
				_sphereData->radiusFixed = p_radius;

			notifyRepresentationDataChange();
		}
	};
	class RepresentationData_VanDerWaals : public RepresentationData
	{
	  public:
		RepresentationData_VanDerWaals();

		float getSphereRadius() const override { return _sphereData->radiusAdd; }
		void  setSphereRadius( float p_radius ) override
		{
			_sphereData->radiusAdd = p_radius;
			notifyRepresentationDataChange();
		}
	};
	class RepresentationData_Sticks : public RepresentationData
	{
	  public:
		RepresentationData_Sticks();

		void setSphereRadius( float p_radius ) override
		{
			_sphereData->radiusFixed = p_radius;
			_cylinderData->radius	 = p_radius;
			notifyRepresentationDataChange();
		}
		void setCylinderRadius( float p_radius ) override
		{
			_sphereData->radiusFixed = p_radius;
			_cylinderData->radius	 = p_radius;
			notifyRepresentationDataChange();
		}
	};
	// !V0.1
	// class RepresentationData_Trace : public RepresentationData
	//{
	//  public:
	//	RepresentationData_Trace();

	//	void setSphereRadius( float p_radius ) override
	//	{
	//		_sphereData->_radiusFixed = p_radius;
	//		_cylinderData->_radius	  = p_radius;
	//		notifyRepresentationDataChange();
	//	}
	//	void setCylinderRadius( float p_radius ) override
	//	{
	//		_sphereData->_radiusFixed = p_radius;
	//		_cylinderData->_radius	  = p_radius;
	//		notifyRepresentationDataChange();
	//	}
	//};
	class RepresentationData_Sas : public RepresentationData
	{
	  public:
		RepresentationData_Sas();
	};

	class RepresentationData_Cartoon : public RepresentationData
	{
	  public:
		RepresentationData_Cartoon();
	};

	class RepresentationData_StickAndCartoon : public RepresentationData
	{
	  public:
		RepresentationData_StickAndCartoon();

		void setSphereRadius( float p_radius ) override
		{
			_sphereData->radiusFixed = p_radius;
			_cylinderData->radius	 = p_radius;
			notifyRepresentationDataChange();
		}
		void setCylinderRadius( float p_radius ) override
		{
			_sphereData->radiusFixed = p_radius;
			_cylinderData->radius	 = p_radius;
			notifyRepresentationDataChange();
		}
	};

	class RepresentationData_Ses : public RepresentationData
	{
	  public:
		RepresentationData_Ses();
	};
} // namespace VTX::Model::Representation
#endif
