#include "app/core/serialization/serialization.hpp"
#include "app/internal/serialization/all_serializers.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Internal::Serialization
{
	// Version
	Util::JSon::Object serialize( const App::Core::Serialization::Version & p_version )
	{
		return { { "MAJOR", p_version.major }, { "MINOR", p_version.minor }, { "REVISION", p_version.revision } };
	}
	void deserialize( const Util::JSon::Object & p_json, App::Core::Serialization::Version & p_version )
	{
		p_version.major	   = SERIALIZER().deserializeField<int>( p_json, "MAJOR" );
		p_version.minor	   = SERIALIZER().deserializeField<int>( p_json, "MINOR" );
		p_version.revision = SERIALIZER().deserializeField<int>( p_json, "REVISION" );
	}

	// VTX App
	Util::JSon::Object serialize( const VTXApp & p_app ) { throw NotImplementedException(); }
	void deserialize( const Util::JSon::Object & p_json, VTXApp & p_app ) { throw NotImplementedException(); }

	// Settings
	Util::JSon::Object serialize( const Application::Setting & p_settings ) { throw NotImplementedException(); }
	void			   deserialize( const Util::JSon::Object & p_json, Application::Setting & p_settings )
	{
		throw NotImplementedException();
	}
} // namespace VTX::App::Internal::Serialization
