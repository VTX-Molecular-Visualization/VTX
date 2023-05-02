#ifndef __VTX_AAPP_APPLICATION_REPRESENTATION_REPRESENTATION_DATA_GENERATOR__
#define __VTX_AAPP_APPLICATION_REPRESENTATION_REPRESENTATION_DATA_GENERATOR__

#include "app/application/representation/_fwd.hpp"
#include "enum_representation.hpp"

namespace VTX::App::Application::Representation
{
	class RepresentationDataGenerator
	{
	  public:
		static RepresentationData * instantiate( const REPRESENTATION_ENUM & p_representationEnum );
	};

} // namespace VTX::App::Application::Representation
#endif
