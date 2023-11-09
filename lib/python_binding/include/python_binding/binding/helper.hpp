#include <magic_enum.hpp>
#include <pybind11/pybind11.h>

namespace VTX::PythonBinding
{
	class Helper
	{
	  public:
		template<typename Enum>
		static void declareEnum( const pybind11::module_ & p_module, const std::string & p_enumName )
		{
			pybind11::enum_<Enum> pyEnum = pybind11::enum_<Enum>( p_module, p_enumName.c_str() );

			for ( const Enum enumValue : magic_enum::enum_values<Enum>() )
			{
				pyEnum.value( magic_enum::enum_name<Enum>( enumValue ).data(), enumValue );
			}

			pyEnum.export_values();
		}
	};
} // namespace VTX::PythonBinding
