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
		class System // : public std::enable_shared_from_this<System>
		{
		  public:
			System();
			~System();

			// std::unique_ptr<Renderer::GL::OpenGLRenderer> _renderer;
			std::unique_ptr<Application::Setting>			   setting		   = nullptr;
			std::unique_ptr<Application::ECS::RegistryManager> registryManager = nullptr;
			std::unique_ptr<Application::ECS::EntityDirector>  entityDirector  = nullptr;
			Application::Scene *							   scene		   = nullptr;
		};
	} // namespace Application
} // namespace VTX::App

#endif
