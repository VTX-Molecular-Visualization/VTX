#ifndef __VTX_APP_APPLICATION_REPRESENTATION_DATA_VAN_DER_WAALS__
#define __VTX_APP_APPLICATION_REPRESENTATION_DATA_VAN_DER_WAALS__

#include "app/application/representation/representation_data.hpp"

namespace VTX::App::Application::Representation::Data
{
	class VanDerWaals : public RepresentationData
	{
	  public:
		VanDerWaals();

		float getSphereRadius() const override { return _sphereData->radiusAdd; }
		void  setSphereRadius( float p_radius ) override
		{
			_sphereData->radiusAdd = p_radius;
			notifyRepresentationDataChange();
		}
	};
} // namespace VTX::App::Application::Representation::Data
#endif
