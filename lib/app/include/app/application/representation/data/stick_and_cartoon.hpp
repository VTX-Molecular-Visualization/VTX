#ifndef __VTX_APP_APPLICATION_REPRESENTATION_DATA_STICK_AND_CARTOON__
#define __VTX_APP_APPLICATION_REPRESENTATION_DATA_STICK_AND_CARTOON__

#include "app/application/representation/representation_data.hpp"

namespace VTX::App::Application::Representation::Data
{
	class StickAndCartoon : public RepresentationData
	{
	  public:
		StickAndCartoon();

		void setSphereRadius( const float p_radius ) override;
		void setCylinderRadius( const float p_radius ) override;
	};

} // namespace VTX::App::Application::Representation::Data
#endif
