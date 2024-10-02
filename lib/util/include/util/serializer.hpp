#ifndef __VTX_UTIL_SERIALIZER__
#define __VTX_UTIL_SERIALIZER__

#include "types.hpp"
#include <util/enum.hpp>
#include <util/json/json.hpp>

namespace VTX::Util::Serializer
{
	// JSon-convertible types
	template<Util::JSon::BasicJSonConcept T>
	Util::JSon::BasicJSon serialize( const T & p_obj )
	{
		return Util::JSon::BasicJSon( p_obj );
	}

	template<Util::JSon::BasicJSonConcept T>
	void deserialize( const Util::JSon::BasicJSon & p_jsonObj, T & p_obj )
	{
		p_obj = p_jsonObj.get<T>();
	}

	// Enums.
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

	// Maths.
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

	// Filesystem.
	std::string serialize( const FilePath & p_filepath ) { return p_filepath.string(); }
	void deserialize( const Util::JSon::BasicJSon & p_json, FilePath & p_filepath ) { p_filepath = p_json.getString(); }

} // namespace VTX::Util::Serializer

#endif
