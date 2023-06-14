#ifndef __VTX_APP_OLD_APPLICATION_REPRESENTATION_DATA_BALL_STICK_AND_CARTOON__
#define __VTX_APP_OLD_APPLICATION_REPRESENTATION_DATA_BALL_STICK_AND_CARTOON__

#include "app/old/application/representation/representation_data.hpp"

namespace VTX::App::Old::Application::Representation::Data
{
	class BallStickAndCartoon : public RepresentationData
	{
	  public:
		BallStickAndCartoon();

		void setSphereRadius( float p_radius ) override;
		void setCylinderRadius( float p_radius ) override;
	};
} // namespace VTX::App::Old::Application::Representation::Data
#endif
