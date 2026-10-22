#include <pybind11/native_enum.h>
#include <pybind11/pybind11.h>
#include <string>
#include <util/enum.hpp>

namespace VTX::PythonBinding
{
	class Helper
	{
	  public:
		template<typename EnumType, typename... Args>
			requires std::is_enum_v<EnumType>
		static void declareEnum( const pybind11::module_ & p_module, const std::string & p_enumName )
		{
			pybind11::native_enum<EnumType> pyEnum
				= pybind11::native_enum<EnumType>( p_module, p_enumName.c_str(), "enum.IntEnum" );

			for ( const EnumType enumValue : magic_enum::enum_values<EnumType>() )
			{
				const std::string enumName( Util::Enum::enumName<EnumType>( enumValue ) );
				pyEnum.value( enumName.c_str(), enumValue );
			}

			pyEnum.finalize();
		}
	};
} // namespace VTX::PythonBinding
