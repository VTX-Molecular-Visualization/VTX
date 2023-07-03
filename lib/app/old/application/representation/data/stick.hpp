#ifndef __VTX_APP_OLD_APPLICATION_REPRESENTATION_DATA_STICK__
#define __VTX_APP_OLD_APPLICATION_REPRESENTATION_DATA_STICK__

#include "app/old/application/representation/representation_data.hpp"

namespace VTX::App::Old::Application::Representation::Data
{
	class Stick : public RepresentationData
	{
	  public:
		Stick();

		void setSphereRadius( const float p_radius ) override;
		void setCylinderRadius( const float p_radius ) override;
	};

} // namespace VTX::App::Old::Application::Representation::Data
#endif
