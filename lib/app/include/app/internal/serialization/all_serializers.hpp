#ifndef __VTX_APP_INTERNAL_SERIALIZATION_ALL_SERIALIZERS__
#define __VTX_APP_INTERNAL_SERIALIZATION_ALL_SERIALIZERS__

#include "app/core/serialization/_fwd.hpp"
#include <concepts>
#include <string>
#include <util/color/rgba.hpp>
#include <util/concepts.hpp>
#include <util/enum.hpp>
#include <util/json/json.hpp>
#include <util/math/aabb.hpp>
#include <util/math/range.hpp>
#include <util/math/range_list.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Internal::Serialization
{
	// Util data structs ////////////////////////////////////////////////////////////////////////////
	template<EnumConcept T>
	std::string serialize( T p_enumValue )
	{
		return std::string( Util::Enum::enumName( p_enumValue ) );
	}
	template<EnumConcept T>
	void deserialize( const Util::JSon::BasicJSon & p_json, T & p_enumValue )
	{
		p_enumValue = Util::Enum::enumCast<T>( p_json.getString() );
	}

	template<typename T, glm::qualifier Q>
	Util::JSon::Object serialize( const glm::vec<2, T, Q> & );
	template<typename T, glm::qualifier Q>
	Util::JSon::Object serialize( const glm::vec<3, T, Q> & );
	template<typename T, glm::qualifier Q>
	Util::JSon::Object serialize( const glm::vec<4, T, Q> & );
	template<typename T, glm::qualifier Q>
	Util::JSon::Object serialize( const glm::qua<T, Q> & );
	template<typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Object &, glm::vec<2, T, Q> & );
	template<typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Object &, glm::vec<3, T, Q> & );
	template<typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Object &, glm::vec<4, T, Q> & );
	template<typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Object &, glm::qua<T, Q> & );

	Util::JSon::Object serialize( const Util::Color::Rgba & );
	void			   deserialize( const Util::JSon::Object & p_json, Util::Color::Rgba & p_color );
	Util::JSon::Object serialize( const Util::Math::Transform & );
	void			   deserialize( const Util::JSon::Object & p_json, Util::Math::Transform & p_color );

	template<std::integral T>
	Util::JSon::Object serialize( const Util::Math::Range<T> & p_range )
	{
		return { { "FIRST", p_range.getFirst() }, { "COUNT", p_range.getCount() } };
	}
	template<std::integral T>
	void deserialize( const Util::JSon::Object & p_json, Util::Math::Range<T> & p_range )
	{
		const T first = p_json[ "FIRST" ].get<T>();
		const T count = p_json[ "COUNT" ].get<T>();

		p_range = Util::Math::Range<T>( first, count );
	}

	template<std::integral T>
	Util::JSon::Array serialize( const Util::Math::RangeList<T> & p_rangeList )
	{
		Util::JSon::Array list = Util::JSon::Array();

		for ( typename Util::Math::RangeList<T>::RangeConstIterator it = p_rangeList.rangeBegin();
			  it != p_rangeList.rangeEnd();
			  it++ )
		{
			list.emplace_back( serialize( *it ) );
		}

		return list;
	}
	template<std::integral T>
	void deserialize( const Util::JSon::Array & p_json, Util::Math::RangeList<T> & p_range )
	{
		for ( const Util::JSon::Object & rangeJSon : p_json )
		{
			Util::Math::Range<T> range;
			deserialize( rangeJSon, range );

			p_range.addRange( range );
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////

	// App data structs
	/////////////////////////////////////////////////////////////////////////////////////////////////
	Util::JSon::Object serialize( const App::Core::Serialization::Version & );
	void			   deserialize( const Util::JSon::Object & p_json, App::Core::Serialization::Version & p_version );
	/////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace VTX::App::Internal::Serialization
#endif
