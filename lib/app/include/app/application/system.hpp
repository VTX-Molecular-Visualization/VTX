#ifndef __VTX_APP_APPLICATION_SYSTEM__
#define __VTX_APP_APPLICATION_SYSTEM__

#include <memory>

namespace VTX::App
{
	namespace Application
	{
		class Scene;
		struct Setting;

		namespace ECS
		{
			class EntityDirector;
			class RegistryManager;
		} // namespace ECS
	}	  // namespace Application

	namespace Core::ECS
	{
		class Registry;
	}

	namespace Application
	{
		class System
		{
		  public:
			System();
			~System();

			std::unique_ptr<Application::Setting>			   setting;
			std::unique_ptr<Application::ECS::RegistryManager> registryManager;
			std::unique_ptr<Application::ECS::EntityDirector>  entityDirector;

			// TODO better way to manage this
			Application::Scene * scene;
		};
	} // namespace Application
} // namespace VTX::App

#endif
