#ifndef __VTX_APP_TEST_UTIL_SERIALIZATION__
#define __VTX_APP_TEST_UTIL_SERIALIZATION__

#include <app/core/serialization/upgrade_utility.hpp>
#include <app/serialization/serialization_system.hpp>
#include <app/vtx_app.hpp>
#include <functional>
#include <string>
#include <util/color/rgba.hpp>
#include <util/constants.hpp>
#include <util/json/json.hpp>
#include <util/types.hpp>

namespace VTX::App::Test::Util::Serialization
{
	class CustomClass
	{
	  public:
		enum class CustomEnum
		{
			ZERO,
			ONE,
			TWO,
			THREE
		};

	  public:
		CustomClass() {}
		CustomClass(
			const VTX::Util::Color::Rgba & p_color,
			const std::string &			   p_strValue,
			const int					   p_intValue,
			const Vec3f &				   p_vector,
			CustomEnum					   p_enum
		) : color( p_color ), strValue( p_strValue ), intValue( p_intValue ), vector( p_vector ), enumValue( p_enum )
		{
		}

		friend bool operator==( const CustomClass & p_lhs, const CustomClass & p_rhs )
		{
			return p_lhs.color == p_rhs.color && p_lhs.strValue == p_rhs.strValue && p_lhs.intValue == p_rhs.intValue
				   && p_lhs.vector == p_rhs.vector && p_lhs.enumValue == p_rhs.enumValue;
		}
		friend bool operator!=( const CustomClass & p_lhs, const CustomClass & p_rhs )
		{
			return p_lhs.color != p_rhs.color || p_lhs.strValue != p_rhs.strValue || p_lhs.intValue != p_rhs.intValue
				   || p_lhs.vector != p_rhs.vector || p_lhs.enumValue != p_rhs.enumValue;
		}

		VTX::Util::Color::Rgba color	 = COLOR_BLACK;
		std::string			   strValue	 = "Default";
		int					   intValue	 = 10;
		Vec3f				   vector	 = VEC3F_ZERO;
		CustomEnum			   enumValue = CustomEnum::ONE;

		static VTX::Util::JSon::Object serialize( const CustomClass & p_obj )
		{
			return { { "COLOR", SERIALIZATION_SYSTEM().serialize( p_obj.color ) },
					 { "STR", SERIALIZATION_SYSTEM().serialize( p_obj.strValue ) },
					 { "INT", SERIALIZATION_SYSTEM().serialize( p_obj.intValue ) },
					 { "VEC", SERIALIZATION_SYSTEM().serialize( p_obj.vector ) },
					 { "ENUM", SERIALIZATION_SYSTEM().serialize( p_obj.enumValue ) } };
		}
		static void deserialize( const VTX::Util::JSon::Object & p_jsonObj, CustomClass & p_obj )
		{
			p_obj.color		= SERIALIZATION_SYSTEM().deserializeField( p_jsonObj, "COLOR", COLOR_BLACK );
			p_obj.strValue	= SERIALIZATION_SYSTEM().deserializeField( p_jsonObj, "STR", std::string( "Default" ) );
			p_obj.intValue	= SERIALIZATION_SYSTEM().deserializeField( p_jsonObj, "INT", 10 );
			p_obj.vector	= SERIALIZATION_SYSTEM().deserializeField( p_jsonObj, "VEC", VEC3F_ZERO );
			p_obj.enumValue = SERIALIZATION_SYSTEM().deserializeField( p_jsonObj, "ENUM", CustomEnum::ONE );
		}

		static void upgrade_0_1_0_to_1_0_0( VTX::Util::JSon::Object & p_jsonObj, CustomClass & p_obj )
		{
			using namespace VTX::App::Core::Serialization;

			UpgradeUtility::renameField( p_jsonObj, "OLD_NAME_COLOR", "COLOR" );
			UpgradeUtility::renameField( p_jsonObj, "OLD_NAME_STR", "STR" );
			UpgradeUtility::renameField( p_jsonObj, "OLD_NAME_INT", "INT" );
			UpgradeUtility::renameField( p_jsonObj, "OLD_NAME_VEC", "VEC" );
			UpgradeUtility::renameField( p_jsonObj, "OLD_NAME_ENUM", "ENUM" );

			// Removed parameter
			// "OLD_UNKNOWN_FIELD"
		}
		static void upgrade_0_0_0_to_0_1_0( VTX::Util::JSon::Object & p_jsonObj, CustomClass & p_obj )
		{
			using namespace VTX::App::Core::Serialization;

			UpgradeUtility::renameField( p_jsonObj, "VERY_OLD_NAME_COLOR", "OLD_NAME_COLOR" );

			// "OLD_NAME_STR" => No changes
			// "OLD_INT" recently added => Nothing to do ; will be filled by default value.

			// Manage type change
			const std::string oldVecStr
				= SERIALIZATION_SYSTEM().deserializeField( p_jsonObj, "OLD_NAME_VEC", std::string( "<Empty>" ) );
			if ( oldVecStr == "Was a string before" )
			{
				const Vec3f convertedVector = { 1.f, 2.f, 3.f };
				p_jsonObj[ "OLD_NAME_VEC" ] = SERIALIZATION_SYSTEM().serialize( convertedVector );
			}

			// Rename VERY_OLD_UNKNOWN_FIELD
			UpgradeUtility::renameField( p_jsonObj, "VERY_OLD_UNKNOWN_FIELD", "OLD_UNKNOWN_FIELD" );

			// "OLD_NAME_ENUM" => No changes
		}
	};

	template<typename T>
	bool checkSerialization( const T & p_obj )
	{
		const VTX::Util::JSon::BasicJSon & json = SERIALIZATION_SYSTEM().serialize( p_obj );

		T deserializedObj;
		SERIALIZATION_SYSTEM().deserialize( json, deserializedObj );

		return p_obj == deserializedObj;
	}

	template<typename T>
	using ComparisonFunction = std::function<bool( const T &, const T & )>;

	template<typename T>
	bool checkSerialization( const T & p_obj, const ComparisonFunction<T> & p_comparisonFunction )
	{
		const VTX::Util::JSon::BasicJSon & json = SERIALIZATION_SYSTEM().serialize( p_obj );

		T deserializedObj;
		SERIALIZATION_SYSTEM().deserialize( json, deserializedObj );

		return p_comparisonFunction( p_obj, deserializedObj );
	}

} // namespace VTX::App::Test::Util::Serialization
#endif
