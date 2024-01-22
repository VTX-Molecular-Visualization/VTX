#ifndef __VTX_APP_TEST_UTIL_APP__
#define __VTX_APP_TEST_UTIL_APP__

#include <app/application/scene.hpp>
#include <app/internal/action/ecs.hpp>
#include <app/vtx_app.hpp>
#include <io/internal/filesystem.hpp>
#include <string>

namespace VTX::App::Test::Util
{
	class App
	{
	  public:
		// std::string MOLECULE_TEST_NAME = "1AGA";
		// std::string MOLECULE_TEST_NAME = "7Y7A";
		inline static const std::string MOLECULE_TEST_NAME	   = "8OIT";
		inline static const std::string MOLECULE_TEST_NAME_EXT = MOLECULE_TEST_NAME + ".mmtf";

		inline static const std::string MOLECULE_TRAJECTORY_TEST_NAME	  = "1NIM";
		inline static const std::string MOLECULE_TRAJECTORY_TEST_NAME_EXT = MOLECULE_TRAJECTORY_TEST_NAME + ".mmtf";

		static void initApp();

		static void loadMolecule( const std::string & p_moleculePath );
		static void loadTestMolecule();
		static void loadTestTrajectoryMolecule();
	};
} // namespace VTX::App::Test::Util
#endif