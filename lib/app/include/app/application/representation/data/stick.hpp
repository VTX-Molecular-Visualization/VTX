#ifndef __VTX_APP_APPLICATION_REPRESENTATION_DATA_STICK__
#define __VTX_APP_APPLICATION_REPRESENTATION_DATA_STICK__

#include "app/application/representation/representation_data.hpp"

namespace VTX::App::Application::Representation::Data
{
	class Stick : public RepresentationData
	{
	  public:
		Stick();

		void setSphereRadius( const float p_radius ) override;
		void setCylinderRadius( const float p_radius ) override;
	};

} // namespace VTX::App::Application::Representation::Data
#endif
