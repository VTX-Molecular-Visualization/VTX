#ifndef __VTX_APP_OLD_APPLICATION_REPRESENTATION_DATA_BALL_AND_STICK__
#define __VTX_APP_OLD_APPLICATION_REPRESENTATION_DATA_BALL_AND_STICK__

#include "app/old/application/representation/representation_data.hpp"

namespace VTX::App::Old::Application::Representation::Data
{
	class BallAndStick : public RepresentationData
	{
	  public:
		BallAndStick();

		void setSphereRadius( float p_radius ) override;
		void setCylinderRadius( float p_radius ) override;
	};

} // namespace VTX::App::Old::Application::Representation::Data
#endif
