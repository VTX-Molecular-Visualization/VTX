#ifndef __VTX_APP_APPLICATION_REPRESENTATION_DATA_BALL_AND_STICK__
#define __VTX_APP_APPLICATION_REPRESENTATION_DATA_BALL_AND_STICK__

#include "app/application/representation/representation_data.hpp"

namespace VTX::App::Application::Representation::Data
{
	class BallAndStick : public RepresentationData
	{
	  public:
		BallAndStick();

		void setSphereRadius( float p_radius ) override;
		void setCylinderRadius( float p_radius ) override;
	};

} // namespace VTX::App::Application::Representation::Data
#endif
