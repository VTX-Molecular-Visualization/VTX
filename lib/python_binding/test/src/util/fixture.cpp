#include "fixture.hpp"
#include <filesystem>
#include <mutex>
#include <util/filesystem.hpp>
#include <util/logger.hpp>

namespace VTX::PythonBinding::Test
{
	Fixture::Fixture()
	{
		static std::once_flag loggerFlag;
		std::call_once(
			loggerFlag,
			[]()
			{
				const FilePath path = Util::Filesystem::getExecutableDir() / "logs";
				std::filesystem::create_directory( path );
				LOGGER::init( path );
			}
		);
	}
} // namespace VTX::PythonBinding::Test
