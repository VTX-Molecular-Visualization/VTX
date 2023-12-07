#ifndef __VTX_APP_INTERNAL_SERIALIZATION_APP_SERIALIZERS__
#define __VTX_APP_INTERNAL_SERIALIZATION_APP_SERIALIZERS__

#include "app/application/_fwd.hpp"
#include "app/core/serialization/version.hpp"
#include <util/json/json.hpp>

namespace VTX::App
{
	class VTXApp;
}

namespace VTX::App::Internal::Serialization
{
	// Version
	Util::JSon::Object serialize( const App::Core::Serialization::Version & );
	void			   deserialize( const Util::JSon::Object &, App::Core::Serialization::Version & );

	// VTXApp
	Util::JSon::Object serialize( const VTXApp & );
	void			   deserialize( const Util::JSon::Object &, VTXApp & );

	// Settings
	Util::JSon::Object serialize( const Application::Setting & );
	void			   deserialize( const Util::JSon::Object &, Application::Setting & );

} // namespace VTX::App::Internal::Serialization
#endif
