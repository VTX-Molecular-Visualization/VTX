#include <util/enum.hpp>
#include <pybind11/pybind11.h>
#include <util/concepts.hpp>

namespace VTX::PythonBinding
{
	class Helper
	{
	  public:
		template<EnumConcept EnumType>
		static void declareEnum( const pybind11::module_ & p_module, const std::string & p_enumName )
		{
			pybind11::enum_<EnumType> pyEnum = pybind11::enum_<EnumType>( p_module, p_enumName.c_str() );

			for ( const EnumType enumValue : magic_enum::enum_values<EnumType>() )
			{
				pyEnum.value( magic_enum::enum_name<EnumType>( enumValue ).data(), enumValue );
			}

			pyEnum.export_values();
		}
	};
} // namespace VTX::PythonBinding
