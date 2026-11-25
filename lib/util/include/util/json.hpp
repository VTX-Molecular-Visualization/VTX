#ifndef VTX_UTIL_JSON_HPP
#define VTX_UTIL_JSON_HPP

#include "util/color/rgba.hpp"
#include "util/filesystem.hpp"
#include "util/types.hpp"
#include <bit>
#include <entt/core/enum.hpp>
#include <magic_enum/magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace VTX::Util::Json
{
	/**
	 * @brief JSon document.
	 */
	using Document = nlohmann::ordered_json;

	/**
	 * @brief Read a JSon document from a file.
	 */
	Document read( const FilePath & p_path );

	/**
	 * @brief Write a JSon document to a file.
	 */
	void write( const FilePath & p_path, const Document & p_document, const int p_indent = 4 );
} // namespace VTX::Util::Json

namespace nlohmann
{
	/**
	 * @brief JSon serializer for Util::Color::Rgba.
	 */
	template<>
	struct adl_serializer<VTX::Util::Color::Rgba>
	{
		template<typename BasicJsonType>
		static void to_json( BasicJsonType & p_json, const VTX::Util::Color::Rgba & p_color )
		{
			const VTX::Vec4f & v = p_color.vec();
			p_json				 = BasicJsonType::array( { v.x, v.y, v.z, v.w } );
		}

		template<typename BasicJsonType>
		static void from_json( const BasicJsonType & p_json, VTX::Util::Color::Rgba & p_color )
		{
			VTX::Vec4f & v = p_color.vec();
			p_json.at( 0 ).get_to( v.x );
			p_json.at( 1 ).get_to( v.y );
			p_json.at( 2 ).get_to( v.z );
			v.w = p_json.size() > 3 ? p_json.at( 3 ).template get<float>() : 1.f;
		}
	};

	/**
	 * @brief JSon serializer for glm::vec.
	 */
	template<glm::length_t L, typename T, glm::qualifier Q>
	struct adl_serializer<glm::vec<L, T, Q>>
	{
		template<typename BasicJsonType>
		static void to_json( BasicJsonType & p_json, const glm::vec<L, T, Q> & p_vec )
		{
			p_json = BasicJsonType::array();
			for ( glm::length_t i = 0; i < L; ++i )
			{
				p_json.push_back( BasicJsonType( p_vec[ i ] ) );
			}
		}

		template<typename BasicJsonType>
		static void from_json( const BasicJsonType & p_json, glm::vec<L, T, Q> & p_vec )
		{
			for ( glm::length_t i = 0; i < L; ++i )
			{
				p_json.at( static_cast<typename BasicJsonType::size_type>( i ) ).get_to( p_vec[ i ] );
			}
		}
	};

	/**
	 * @brief JSon serializer for glm::mat.
	 */
	template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
	struct adl_serializer<glm::mat<C, R, T, Q>>
	{
		template<typename BasicJsonType>
		static void to_json( BasicJsonType & p_json, const glm::mat<C, R, T, Q> & p_mat )
		{
			p_json = BasicJsonType::array();
			for ( glm::length_t i = 0; i < C; ++i )
			{
				p_json.push_back( BasicJsonType( p_mat[ i ] ) );
			}
		}

		template<typename BasicJsonType>
		static void from_json( const BasicJsonType & p_json, glm::mat<C, R, T, Q> & p_mat )
		{
			for ( glm::length_t i = 0; i < C; ++i )
			{
				p_json.at( static_cast<typename BasicJsonType::size_type>( i ) ).get_to( p_mat[ i ] );
			}
		}
	};

	/**
	 * @brief JSon serializer for glm::qua.
	 */
	template<typename T, glm::qualifier Q>
	struct adl_serializer<glm::qua<T, Q>>
	{
		template<typename BasicJsonType>
		static void to_json( BasicJsonType & p_json, const glm::qua<T, Q> & p_quat )
		{ p_json = BasicJsonType::array( { p_quat.x, p_quat.y, p_quat.z, p_quat.w } ); }

		template<typename BasicJsonType>
		static void from_json( const BasicJsonType & p_json, glm::qua<T, Q> & p_quat )
		{
			p_json.at( 0 ).get_to( p_quat.x );
			p_json.at( 1 ).get_to( p_quat.y );
			p_json.at( 2 ).get_to( p_quat.z );
			p_json.at( 3 ).get_to( p_quat.w );
		}
	};

	/**
	 * @brief JSon serializer for enums.
	 */
	template<typename E>
	struct adl_serializer<E, std::enable_if_t<std::is_enum_v<E>>>
	{
		template<typename BasicJsonType>
		static void to_json( BasicJsonType & p_json, const E p_value )
		{
			// Array of enums for bitmasks.
			if constexpr ( entt::enum_as_bitmask_v<E> )
			{
				const Bits value = static_cast<Bits>( magic_enum::enum_integer( p_value ) );
				p_json			 = BasicJsonType::array();
				for ( const E flag : magic_enum::enum_values<E>() )
				{
					const Bits bit = static_cast<Bits>( magic_enum::enum_integer( flag ) );
					if ( std::has_single_bit( bit ) && ( value & bit ) == bit )
					{
						p_json.push_back( std::string( magic_enum::enum_name( flag ) ) );
					}
				}
			}
			// Single enum value.
			else
			{
				p_json = std::string( magic_enum::enum_name( p_value ) );
			}
		}

		template<typename BasicJsonType>
		static void from_json( const BasicJsonType & p_json, E & p_value )
		{
			if constexpr ( entt::enum_as_bitmask_v<E> )
			{
				Bits value = 0;
				for ( const auto & item : p_json )
				{
					value |= static_cast<Bits>( magic_enum::enum_integer( _cast( item ) ) );
				}
				p_value = static_cast<E>( value );
			}
			else
			{
				p_value = _cast( p_json );
			}
		}

	  private:
		using Bits = std::make_unsigned_t<std::underlying_type_t<E>>;

		template<typename BasicJsonType>
		static E _cast( const BasicJsonType & p_json )
		{
			const std::string name	= p_json.template get<std::string>();
			const auto		  value = magic_enum::enum_cast<E>( name );
			if ( not value )
			{
				throw std::invalid_argument(
					"Invalid value \"" + name + "\" for enum " + std::string( magic_enum::enum_type_name<E>() )
				);
			}
			return *value;
		}
	};
} // namespace nlohmann

#endif
