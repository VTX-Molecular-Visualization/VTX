#include "app/core/serialization/util_serializers.hpp"
#include "app/serialization/serialization_system.hpp"

namespace VTX::App::Core::Serialization
{
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
		p_color.setR( SERIALIZATION_SYSTEM().deserializeField( p_json, "R", 0.f ) );
		p_color.setG( SERIALIZATION_SYSTEM().deserializeField( p_json, "G", 0.f ) );
		p_color.setB( SERIALIZATION_SYSTEM().deserializeField( p_json, "B", 0.f ) );
		p_color.setA( SERIALIZATION_SYSTEM().deserializeField( p_json, "A", 1.f ) );
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
		const Vec3f position = SERIALIZATION_SYSTEM().deserializeField( p_json, "POSITION", VEC3F_ZERO );
		p_transform.setTranslation( position );

		const Vec3f euler = SERIALIZATION_SYSTEM().deserializeField( p_json, "ROTATION", VEC3F_ZERO );
		p_transform.setRotation( euler );

		const Vec3f scale = SERIALIZATION_SYSTEM().deserializeField( p_json, "SCALE", VEC3F_XYZ );
		p_transform.setScale( scale );
	}
} // namespace VTX::App::Core::Serialization
