#ifndef __VTX_APP_INTERNAL_SERIALIZATION_ALL_SERIALIZERS__
#define __VTX_APP_INTERNAL_SERIALIZATION_ALL_SERIALIZERS__

#include "app/core/serialization/_fwd.hpp"
#include <string>
#include <util/color/rgba.hpp>
#include <util/concepts.hpp>
#include <util/enum.hpp>
#include <util/json/json.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Internal::Serialization
{
	// Serialization ////////////////////////////////////////////////////////////////////////////////

	// Util data structs
	template<EnumConcept T>
	std::string serialize( T p_enumValue )
	{
		return std::string( Util::Enum::enumName( p_enumValue ) );
	}

	template<typename T, glm::qualifier Q>
	Util::JSon::Object serialize( const glm::vec<2, T, Q> & );
	template<typename T, glm::qualifier Q>
	Util::JSon::Object serialize( const glm::vec<3, T, Q> & );
	template<typename T, glm::qualifier Q>
	Util::JSon::Object serialize( const glm::vec<4, T, Q> & );
	template<typename T, glm::qualifier Q>
	Util::JSon::Object serialize( const glm::qua<T, Q> & );

	Util::JSon::Object serialize( const Util::Color::Rgba & );
	Util::JSon::Object serialize( const Util::Math::Transform & );

	// App data structs
	Util::JSon::Object serialize( const App::Core::Serialization::Version & );
	/////////////////////////////////////////////////////////////////////////////////////////////////

	// Deserialization /////////////////////////////////////////////////////////////////////////////

	template<EnumConcept T>
	void deserialize( const Util::JSon::BasicJSon & p_json, T & p_enumValue )
	{
		p_enumValue = Util::Enum::enumCast<T>( p_json.getString() );
	}
	// Util data structs
	template<typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Object &, glm::vec<2, T, Q> & );
	template<typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Object &, glm::vec<3, T, Q> & );
	template<typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Object &, glm::vec<4, T, Q> & );
	template<typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Object &, glm::qua<T, Q> & );

	void deserialize( const Util::JSon::Object & p_json, Util::Color::Rgba & p_color );
	void deserialize( const Util::JSon::Object & p_json, Util::Math::Transform & p_color );

	// App data structs
	void deserialize( const Util::JSon::Object & p_json, App::Core::Serialization::Version & p_version );

	/////////////////////////////////////////////////////////////////////////////////////////////////

	//_migrate( const nlohmann::json &, const std::tuple<uint, uint, uint> &,
	// Application::Representation::InstantiatedRepresentation & ) 	const; void _migrate( const nlohmann::json &,
	// const std::tuple<uint, uint, uint> &, Application::Representation::RepresentationPreset & ) 	const;
} // namespace VTX::App::Internal::Serialization

#endif
