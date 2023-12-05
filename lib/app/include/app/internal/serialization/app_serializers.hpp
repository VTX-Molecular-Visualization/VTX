#ifndef __VTX_APP_INTERNAL_SERIALIZATION_APP_SERIALIZERS__
#define __VTX_APP_INTERNAL_SERIALIZATION_APP_SERIALIZERS__

#include "app/core/serialization/version.hpp"
#include <util/json/json.hpp>

namespace VTX::App::Internal::Serialization
{
	// App data structs /////////////////////////////////////////////////////////////////////////////
	// Version
	Util::JSon::Object serialize( const App::Core::Serialization::Version & );
	void			   deserialize( const Util::JSon::Object & p_json, App::Core::Serialization::Version & p_version );
	/////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace VTX::App::Internal::Serialization
#endif
