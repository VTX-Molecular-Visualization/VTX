#ifndef __VTX_APP_OLD_APPLICATION_REPRESENTATION_DATA_STICK_AND_CARTOON__
#define __VTX_APP_OLD_APPLICATION_REPRESENTATION_DATA_STICK_AND_CARTOON__

#include "app/old/application/representation/representation_data.hpp"

namespace VTX::App::Old::Application::Representation::Data
{
	class StickAndCartoon : public RepresentationData
	{
	  public:
		StickAndCartoon();

		void setSphereRadius( const float p_radius ) override;
		void setCylinderRadius( const float p_radius ) override;
	};

} // namespace VTX::App::Old::Application::Representation::Data
#endif
