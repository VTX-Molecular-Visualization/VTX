#ifndef ___VTX_APP_VTX_APP___
#define ___VTX_APP_VTX_APP___

#include "app/application/system.hpp"
#include "application/_fwd.hpp"
#include "core/ecs/_fwd.hpp"
// #include <QElapsedTimer>
#include <memory>
#include <string>
#include <util/exceptions.hpp>
#include <util/generic/base_static_singleton.hpp>
#include <vector>

namespace VTX
{
	namespace Renderer
	{
		class Renderer;
	}

	namespace App
	{
		class VTXApp : public Util::Generic::BaseStaticSingleton<VTXApp> // final
		{
		  private:
			inline static const std::string REGISTRY_MANAGER_KEY  = "REGISTRY_MANAGER";
			inline static const std::string SETTING_KEY			  = "SETTING";
			inline static const std::string ENTITY_DIRECTOR_KEY	  = "ENTITY_DIRECTOR";
			inline static const std::string SCENE_KEY			  = "SCENE";
			inline static const std::string SELECTION_MANAGER_KEY = "SELECTION_MANAGER";

		  public:
			VTXApp( StructPrivacyToken );
			VTXApp( std::initializer_list<int> ) = delete;
			VTXApp( const VTXApp & )			 = delete;
			VTXApp & operator=( const VTXApp & ) = delete;
			~VTXApp();

			void start( const std::vector<std::string> & );
			void update();
			void goToState( const std::string &, void * const = nullptr );
			void stop();

			inline const Application::System &			getSystem() const { return *_system; };
			inline Application::System &				getSystem() { return *_system; };
			inline std::shared_ptr<Application::System> getSystemPtr() { return _system; };

			inline void referenceSystem( std::shared_ptr<Application::System> p_system ) { _system = p_system; };

			Application::Scene &	   getScene();
			const Application::Scene & getScene() const;

			inline Renderer::Renderer &		  getRenderer() { return *_renderer; }
			inline const Renderer::Renderer & getRenderer() const { return *_renderer; }

			Application::Setting &		 getSettings();
			const Application::Setting & getSettings() const;

			Application::ECS::RegistryManager &		  getRegistryManager();
			const Application::ECS::RegistryManager & getRegistryManager() const;

			Application::ECS::EntityDirector & getEntityDirector();

			Application::Selection::SelectionManager &		 getSelectionManager();
			const Application::Selection::SelectionManager & getSelectionManager() const;

		  private:
			std::shared_ptr<Application::System> _system = nullptr;
			std::unique_ptr<Renderer::Renderer>	 _renderer;

			std::unique_ptr<Application::Setting>					  _setting;
			std::unique_ptr<Application::ECS::RegistryManager>		  _registryManager;
			std::unique_ptr<Application::ECS::EntityDirector>		  _entityDirector;
			std::unique_ptr<Application::Selection::SelectionManager> _selectionManager;

			void _handleArgs( const std::vector<std::string> & );
			void _update();
			void _stop();
		};

		// Convenient accessors
		Application::Scene &				SCENE();
		Core::ECS::Registry &				MAIN_REGISTRY();
		Application::Selection::Selection & CURRENT_SELECTION();
	} // namespace App
} // namespace VTX

#endif
