#pragma once

#include <pybind11/native_enum.h>
#include <pybind11/pybind11.h>
#include <string>
#include <string_view>
#include <util/enum.hpp>

namespace VTX::PythonBinding
{
	class Helper
	{
	  public:
		template<typename EnumType>
			requires std::is_enum_v<EnumType>
		static void declareEnum(
			const pybind11::module_ & p_module,
			const std::string &		  p_enumName,
			const std::string &		  p_doc,
			const std::string_view	  p_valuePrefix = {}
		)
		{
			pybind11::native_enum<EnumType> pyEnum
				= pybind11::native_enum<EnumType>( p_module, p_enumName.c_str(), "enum.IntEnum", p_doc.c_str() );

			for ( const EnumType enumValue : magic_enum::enum_values<EnumType>() )
			{
				if constexpr ( requires { EnumType::COUNT; } )
				{
					if ( enumValue == EnumType::COUNT )
					{
						continue;
					}
				}

				std::string_view enumName = Util::Enum::enumName<EnumType>( enumValue );
				if ( not p_valuePrefix.empty() && enumName.starts_with( p_valuePrefix ) )
				{
					enumName.remove_prefix( p_valuePrefix.size() );
				}
				pyEnum.value( std::string( enumName ).c_str(), enumValue );
			}

			pyEnum.finalize();
		}
	};
} // namespace VTX::PythonBinding
