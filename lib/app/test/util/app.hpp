#ifndef __VTX_APP_TEST_UTIL_APP__
#define __VTX_APP_TEST_UTIL_APP__

#include <app/application/scene.hpp>
#include <app/core/ecs/base_entity.hpp>
#include <io/internal/filesystem.hpp>
#include <string>
#include <vector>

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

		static void loadSystem( const std::string & p_systemPath );
		static void loadTestSystem();
		static void loadTestTrajectorySystem();

		static bool checkItemOrder(
			const VTX::App::Application::Scene &	   p_scene,
			const std::vector<Core::ECS::BaseEntity> & p_entities
		);
	};
} // namespace VTX::App::Test::Util
#endif
