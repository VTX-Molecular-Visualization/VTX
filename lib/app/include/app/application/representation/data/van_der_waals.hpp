#ifndef __VTX_APP_APPLICATION_REPRESENTATION_DATA_VAN_DER_WAALS__
#define __VTX_APP_APPLICATION_REPRESENTATION_DATA_VAN_DER_WAALS__

#include "app/application/representation/representation_data.hpp"

namespace VTX::App::Application::Representation::Data
{
	class VanDerWaals : public RepresentationData
	{
	  public:
		VanDerWaals();

		// inline float getSphereRadius() const override { return _sphereData->radiusAdd; }
		void setSphereRadius( const float p_radius ) override;
	};
} // namespace VTX::App::Application::Representation::Data
#endif
