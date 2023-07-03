#ifndef __VTX_APP_OLD_APPLICATION_REPRESENTATION_DATA_VAN_DER_WAALS__
#define __VTX_APP_OLD_APPLICATION_REPRESENTATION_DATA_VAN_DER_WAALS__

#include "app/old/application/representation/representation_data.hpp"

namespace VTX::App::Old::Application::Representation::Data
{
	class VanDerWaals : public RepresentationData
	{
	  public:
		VanDerWaals();

		// inline float getSphereRadius() const override { return _sphereData->radiusAdd; }
		void setSphereRadius( const float p_radius ) override;
	};
} // namespace VTX::App::Old::Application::Representation::Data
#endif
