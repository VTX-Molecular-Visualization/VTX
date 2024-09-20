#ifndef __VTX_APP_SERIALIZATION_UTIL_SERIALIZERS__
#define __VTX_APP_SERIALIZATION_UTIL_SERIALIZERS__

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

namespace VTX::App::Serialization
{
	// Enum
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

	// Types
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
	template<int M, int N, typename T, glm::qualifier Q>
	Util::JSon::Array serialize( const glm::mat<M, N, T, Q> & p_mat )
	{
		Util::JSon::Array res;

		for ( int i = 0; i < M; i++ )
		{
			for ( int j = 0; j < N; j++ )
			{
				res.emplace_back( p_mat[ i ][ j ] );
			}
		}

		return res;
	}

	template<typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Object & p_json, glm::vec<2, T, Q> & p_vec )
	{
		p_vec.x = p_json[ "X" ].get<T>();
		p_vec.y = p_json[ "Y" ].get<T>();
	}
	template<typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Object & p_json, glm::vec<3, T, Q> & p_vec )
	{
		p_vec.x = p_json[ "X" ].get<T>();
		p_vec.y = p_json[ "Y" ].get<T>();
		p_vec.z = p_json[ "Z" ].get<T>();
	}
	template<typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Object & p_json, glm::vec<4, T, Q> & p_vec )
	{
		p_vec.x = p_json[ "X" ].get<T>();
		p_vec.y = p_json[ "Y" ].get<T>();
		p_vec.z = p_json[ "Z" ].get<T>();
		p_vec.w = p_json[ "W" ].get<T>();
	}
	template<typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Object & p_json, glm::qua<T, Q> & p_quat )
	{
		p_quat.x = p_json[ "X" ].get<T>();
		p_quat.y = p_json[ "Y" ].get<T>();
		p_quat.z = p_json[ "Z" ].get<T>();
		p_quat.w = p_json[ "W" ].get<T>();
	}
	template<int M, int N, typename T, glm::qualifier Q>
	void deserialize( const Util::JSon::Array & p_json, glm::mat<M, N, T, Q> & p_mat )
	{
		for ( int i = 0; i < M; i++ )
		{
			for ( int j = 0; j < N; j++ )
			{
				T value			= p_json[ i * N + j ].get<T>();
				p_mat[ i ][ j ] = value;
			}
		}
	}

	// Filesystem
	std::string serialize( const FilePath & );
	void		deserialize( const Util::JSon::BasicJSon & p_json, FilePath & p_filepath );

	// Math - Color
	Util::JSon::Object serialize( const Util::Color::Rgba & );
	void			   deserialize( const Util::JSon::Object & p_json, Util::Color::Rgba & p_color );
	// Math - Transform
	Util::JSon::Object serialize( const Util::Math::Transform & );
	void			   deserialize( const Util::JSon::Object & p_json, Util::Math::Transform & p_color );

	// Math - Range
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

	// Math - RangeList
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

} // namespace VTX::App::Serialization
#endif
