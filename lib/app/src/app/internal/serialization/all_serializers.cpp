#include "app/internal/serialization/all_serializers.hpp"
#include "app/core/serialization/serialization.hpp"
#include "app/core/serialization/version.hpp"
#include "app/info.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Internal::Serialization
{
	// Util data structs ////////////////////////////////////////////////////////////////////////////
	// Filesystem
	std::string serialize( const FilePath & p_filepath ) { return p_filepath.string(); }
	void deserialize( const Util::JSon::BasicJSon & p_json, FilePath & p_filepath ) { p_filepath = p_json.getString(); }

	// Math - Color
	Util::JSon::Object serialize( const Util::Color::Rgba & p_color )
	{
		return { { "R", p_color.getR() }, { "G", p_color.getG() }, { "B", p_color.getB() }, { "A", p_color.getA() } };
	}
	void deserialize( const Util::JSon::Object & p_json, Util::Color::Rgba & p_color )
	{
		p_color.setR( SERIALIZER().deserializeField( p_json, "R", 0.f ) );
		p_color.setG( SERIALIZER().deserializeField( p_json, "G", 0.f ) );
		p_color.setB( SERIALIZER().deserializeField( p_json, "B", 0.f ) );
		p_color.setA( SERIALIZER().deserializeField( p_json, "A", 1.f ) );
	}

	// Math - Transform
	Util::JSon::Object serialize( const Util::Math::Transform & p_transform )
	{
		return { { "POSITION", serialize( p_transform.getTranslationVector() ) },
				 { "ROTATION", serialize( p_transform.getEulerAngles() ) },
				 { "SCALE", serialize( p_transform.getScaleVector() ) } };
	}
	void deserialize( const Util::JSon::Object & p_json, Util::Math::Transform & p_transform )
	{
		const Vec3f position = SERIALIZER().deserializeField( p_json, "POSITION", VEC3F_ZERO );
		p_transform.setTranslation( position );

		const Vec3f euler = SERIALIZER().deserializeField( p_json, "ROTATION", VEC3F_ZERO );
		p_transform.setRotation( euler );

		const Vec3f scale = SERIALIZER().deserializeField( p_json, "SCALE", VEC3F_XYZ );
		p_transform.setScale( scale );
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////

	// App data structs /////////////////////////////////////////////////////////////////////////////
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
	/////////////////////////////////////////////////////////////////////////////////////////////////
} // namespace VTX::App::Internal::Serialization
