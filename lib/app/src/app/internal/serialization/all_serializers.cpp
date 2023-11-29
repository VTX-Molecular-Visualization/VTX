#include "app/internal/serialization/all_serializers.hpp"
#include "app/core/serialization/serialization.hpp"
#include "app/core/serialization/version.hpp"
#include "app/info.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Internal::Serialization
{
	Util::JSon::Object serialize( const Util::Color::Rgba & p_color )
	{
		return { { "R", p_color.getR() }, { "G", p_color.getG() }, { "B", p_color.getB() }, { "A", p_color.getA() } };
	}
	Util::JSon::Object serialize( const Util::Math::Transform & p_transform )
	{
		return { { "POSITION", serialize( p_transform.getTranslationVector() ) },
				 { "ROTATION", serialize( p_transform.getEulerAngles() ) },
				 { "SCALE", serialize( p_transform.getScaleVector() ) } };
	}

	Util::JSon::Object serialize( const App::Core::Serialization::Version & p_version )
	{
		return { { "MAJOR", p_version.major }, { "MINOR", p_version.minor }, { "REVISION", p_version.revision } };
	}

	template<typename T, glm::qualifier Q>
	Util::JSon::Object serialize( const glm::vec<2, T, Q> & p_vec )
	{
		return Util::JSon::Object( { { "X", p_vec.x }, { "Y", p_vec.y } } );
	}
	template<typename T, glm::qualifier Q>
	Util::JSon::Object serialize( const glm::vec<3, T, Q> & p_vec )
	{
		return Util::JSon::Object( { { "X", p_vec.x }, { "Y", p_vec.y }, { "Z", p_vec.z } } );
	}
	template<typename T, glm::qualifier Q>
	Util::JSon::Object serialize( const glm::vec<4, T, Q> & p_vec )
	{
		return Util::JSon::Object( { { "X", p_vec.x }, { "Y", p_vec.y }, { "Z", p_vec.z }, { "W", p_vec.w } } );
	}

	template<typename T, glm::qualifier Q>
	Util::JSon::Object serialize( const glm::qua<T, Q> & p_quat )
	{
		return Util::JSon::Object( { { "X", p_quat.x }, { "Y", p_quat.y }, { "Z", p_quat.z }, { "W", p_quat.w } } );
	}

	template<typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Object & p_json, glm::vec<2, T, Q> & p_vec )
	{
		p_vec.x = SERIALIZER().deserializeField<T>( p_json, "X" );
		p_vec.y = SERIALIZER().deserializeField<T>( p_json, "Y" );
	}
	template<typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Object & p_json, glm::vec<3, T, Q> & p_vec )
	{
		p_vec.x = SERIALIZER().deserializeField<T>( p_json, "X" );
		p_vec.y = SERIALIZER().deserializeField<T>( p_json, "Y" );
		p_vec.z = SERIALIZER().deserializeField<T>( p_json, "Z" );
	}
	template<typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Object & p_json, glm::vec<4, T, Q> & p_vec )
	{
		p_vec.x = SERIALIZER().deserializeField<T>( p_json, "X" );
		p_vec.y = SERIALIZER().deserializeField<T>( p_json, "Y" );
		p_vec.z = SERIALIZER().deserializeField<T>( p_json, "Z" );
		p_vec.w = SERIALIZER().deserializeField<T>( p_json, "W" );
	}

	template<typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Object & p_json, glm::qua<T, Q> & p_quat )
	{
		p_quat.x = SERIALIZER().deserializeField<T>( p_json, "X" );
		p_quat.y = SERIALIZER().deserializeField<T>( p_json, "Y" );
		p_quat.z = SERIALIZER().deserializeField<T>( p_json, "Z" );
		p_quat.w = SERIALIZER().deserializeField<T>( p_json, "W" );
	}

	void deserialize( const Util::JSon::Object & p_json, Util::Color::Rgba & p_color )
	{
		p_color.setR( SERIALIZER().deserializeField( p_json, "R", 0.f ) );
		p_color.setG( SERIALIZER().deserializeField( p_json, "G", 0.f ) );
		p_color.setB( SERIALIZER().deserializeField( p_json, "B", 0.f ) );
		p_color.setA( SERIALIZER().deserializeField( p_json, "A", 1.f ) );
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

	void deserialize( const Util::JSon::Object & p_json, App::Core::Serialization::Version & p_version )
	{
		p_version.major	   = SERIALIZER().deserializeField<int>( p_json, "MAJOR" );
		p_version.minor	   = SERIALIZER().deserializeField<int>( p_json, "MINOR" );
		p_version.revision = SERIALIZER().deserializeField<int>( p_json, "REVISION" );
	}
} // namespace VTX::App::Internal::Serialization
