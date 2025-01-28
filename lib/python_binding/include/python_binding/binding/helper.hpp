#include <pybind11/pybind11.h>
#include <util/concepts.hpp>
#include <util/enum.hpp>

namespace VTX::PythonBinding
{
	class Helper
	{
	  public:
		template<EnumConcept EnumType, typename... Args>
		static void declareEnum( const pybind11::module_ & p_module, const std::string & p_enumName, Args &&... args )
		{
			pybind11::enum_<EnumType> pyEnum
				= pybind11::enum_<EnumType>( p_module, p_enumName.c_str(), std::forward<Args>( args )... );

			for ( const EnumType enumValue : magic_enum::enum_values<EnumType>() )
			{
				pyEnum.value( magic_enum::enum_name<EnumType>( enumValue ).data(), enumValue );
			}

			pyEnum.export_values();
		}
	};
} // namespace VTX::PythonBinding
