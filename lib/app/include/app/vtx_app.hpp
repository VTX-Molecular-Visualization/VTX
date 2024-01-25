#ifndef ___VTX_APP_VTX_APP___
#define ___VTX_APP_VTX_APP___

#include "app/application/system.hpp"
#include "app/core/callback_event.hpp"
#include "app/core/monitoring/stats.hpp"
#include "application/_fwd.hpp"
#include "core/ecs/_fwd.hpp"
#include "core/serialization/_fwd.hpp"
#include "core/worker/_fwd.hpp"
#include <memory>
#include <string>
#include <util/chrono.hpp>
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
		class VTXApp final : public Util::Generic::BaseStaticSingleton<VTXApp>
		{
		  private:
			inline static const std::string REGISTRY_MANAGER_KEY   = "REGISTRY_MANAGER";
			inline static const std::string SETTINGS_KEY		   = "SETTINGS";
			inline static const std::string ENTITY_DIRECTOR_KEY	   = "ENTITY_DIRECTOR";
			inline static const std::string SCENE_KEY			   = "SCENE";
			inline static const std::string SELECTION_MANAGER_KEY  = "SELECTION_MANAGER";
			inline static const std::string SERIALIZATION_TOOL_KEY = "SERIALIZATION_TOOL";
			inline static const std::string ACTION_MANAGER_KEY	   = "ACTION_MANAGER";
			inline static const std::string WORKER_MANAGER_KEY	   = "WORKER_MANAGER";

		  public:
			VTXApp( StructPrivacyToken );
			VTXApp( std::initializer_list<int> ) = delete;
			VTXApp( const VTXApp & )			 = delete;
			VTXApp & operator=( const VTXApp & ) = delete;
			~VTXApp();

			void start( const std::vector<std::string> & );
			void update( const float p_elapsedTime = 0 );
			void goToState( const std::string &, void * const = nullptr );
			void stop();

			Core::CallbackEmitter<float> & onPreUpdate() { return _preUpdateCallback; };
			Core::CallbackEmitter<float> & onUpdate() { return _updateCallback; };
			Core::CallbackEmitter<float> & onLateUpdate() { return _lateUpdateCallback; };
			Core::CallbackEmitter<float> & onPostUpdate() { return _postUpdateCallback; };

			Core::CallbackEmitter<float> & onPreRender() { return _preRenderCallback; };
			Core::CallbackEmitter<float> & onRender() { return _renderCallback; };
			Core::CallbackEmitter<float> & onPostRender() { return _postRenderCallback; };

			inline const Application::System &			getSystem() const { return *_system; };
			inline Application::System &				getSystem() { return *_system; };
			inline std::shared_ptr<Application::System> getSystemPtr() { return _system; };

			inline void referenceSystem( std::shared_ptr<Application::System> p_system ) { _system = p_system; };

			const Core::Monitoring::Stats & getStats() const { return _stats; }
			Core::Monitoring::Stats &		getStats() { return _stats; }

			Application::Scene &	   getScene();
			const Application::Scene & getScene() const;

			inline Renderer::Renderer &		  getRenderer() { return *_renderer; }
			inline const Renderer::Renderer & getRenderer() const { return *_renderer; }

			Application::Settings &		  getSettings();
			const Application::Settings & getSettings() const;

			Application::ECS::RegistryManager &		  getRegistryManager();
			const Application::ECS::RegistryManager & getRegistryManager() const;

			Application::ECS::EntityDirector & getEntityDirector();

			Application::Selection::SelectionManager &		 getSelectionManager();
			const Application::Selection::SelectionManager & getSelectionManager() const;

			Core::Serialization::Serialization &	   getSerializationTool();
			const Core::Serialization::Serialization & getSerializationTool() const;

			Application::Action::ActionManager &	   getActionManager();
			const Application::Action::ActionManager & getActionManager() const;

			Core::Worker::WorkerManager &		getWorkerManager();
			const Core::Worker::WorkerManager & getWorkerManager() const;

		  private:
			Util::Chrono _tickChrono = Util::Chrono();

			std::shared_ptr<Application::System> _system = std::make_shared<Application::System>();

			std::unique_ptr<Renderer::Renderer> _renderer;

			std::unique_ptr<Application::Settings>					  _settings;
			std::unique_ptr<Application::ECS::RegistryManager>		  _registryManager;
			std::unique_ptr<Application::ECS::EntityDirector>		  _entityDirector;
			std::unique_ptr<Application::Selection::SelectionManager> _selectionManager;
			std::unique_ptr<Core::Serialization::Serialization>		  _serializationToolManager;
			std::unique_ptr<Application::Action::ActionManager>		  _actionManager;
			std::unique_ptr<Core::Worker::WorkerManager>			  _workerManager;

			Core::Monitoring::Stats _stats = Core::Monitoring::Stats();

			Core::CallbackEmitter<float> _preUpdateCallback	 = Core::CallbackEmitter<float>();
			Core::CallbackEmitter<float> _updateCallback	 = Core::CallbackEmitter<float>();
			Core::CallbackEmitter<float> _lateUpdateCallback = Core::CallbackEmitter<float>();
			Core::CallbackEmitter<float> _postUpdateCallback = Core::CallbackEmitter<float>();

			Core::CallbackEmitter<float> _preRenderCallback	 = Core::CallbackEmitter<float>();
			Core::CallbackEmitter<float> _renderCallback	 = Core::CallbackEmitter<float>();
			Core::CallbackEmitter<float> _postRenderCallback = Core::CallbackEmitter<float>();

			void _handleArgs( const std::vector<std::string> & );
			void _applyCameraUniforms() const;
			void _update( const float p_elapsedTime );
			void _stop();
		};

		// Convenient accessors
		Application::Scene &				 SCENE();
		Renderer::Renderer &				 RENDERER();
		Application::Settings &				 SETTINGS();
		Application::ECS::RegistryManager &	 MAIN_REGISTRY();
		Application::Selection::Selection &	 CURRENT_SELECTION();
		Core::Serialization::Serialization & SERIALIZER();
		Core::Worker::WorkerManager &		 THREADING();
		Application::Action::ActionManager & VTX_ACTION();
	} // namespace App
} // namespace VTX

#endif
