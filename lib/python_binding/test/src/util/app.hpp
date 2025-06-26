#ifndef __VTX_APP_TEST_UTIL_APP__
#define __VTX_APP_TEST_UTIL_APP__

#include <io/internal/filesystem.hpp>
#include <memory>
#include <python_binding/interpretor.hpp>
#include <string>
#include <util/singleton.hpp>

namespace VTX
{
	inline VTX::PythonBinding::Interpretor & INTERPRETOR()
	{
		return Util::Singleton<VTX::PythonBinding::Interpretor>::get();
	}
} // namespace VTX
namespace VTX::App::Test::Util
{

	class PythonFixture
	{
	  public:
		inline static const std::string MOLECULE_TEST_NAME	   = "1AGA";
		inline static const std::string MOLECULE_TEST_NAME_EXT = MOLECULE_TEST_NAME + ".mmtf";

		static void loadSystem( const std::string & p_systemPath );
		static void loadTestSystem();

		PythonFixture();
		~PythonFixture();

		void resetInterpretor();
	};
} // namespace VTX::App::Test::Util
#endif
