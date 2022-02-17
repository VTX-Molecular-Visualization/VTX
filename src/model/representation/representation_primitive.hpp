#ifndef __VTX_MODEL_REPRESENTATION_PRIMITIVE__
#define __VTX_MODEL_REPRESENTATION_PRIMITIVE__

namespace VTX::Model::Representation
{

	class SphereData
	{
	  public:
		SphereData() {};

		float _radiusFixed	 = 1.0f;
		float _radiusAdd	 = 0.f;
		bool  _isRadiusFixed = true;
	};

	class CylinderData
	{
	  public:
		CylinderData() {};
		float _radius = 1.0f;
	};

	class RibbonData
	{
	  public:
		RibbonData() {};
	};

} // namespace VTX::Model::Representation
#endif
