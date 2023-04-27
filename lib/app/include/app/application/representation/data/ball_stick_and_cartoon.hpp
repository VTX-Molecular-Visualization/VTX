#ifndef __VTX_APP_APPLICATION_REPRESENTATION_DATA_BALL_STICK_AND_CARTOON__
#define __VTX_APP_APPLICATION_REPRESENTATION_DATA_BALL_STICK_AND_CARTOON__

#include "app/application/representation/representation_data.hpp"

namespace VTX::App::Application::Representation::Data
{
	class BallStickAndCartoon : public RepresentationData
	{
	  public:
		BallStickAndCartoon();

		void setSphereRadius( float p_radius ) override;
		void setCylinderRadius( float p_radius ) override;
	};
} // namespace VTX::App::Application::Representation::Data
#endif
