#ifndef ___VTX_APP_VTX_APP___
#define ___VTX_APP_VTX_APP___

#include "app/application/system.hpp"
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
			inline std::shared_ptr<Application::System> getSystemPtr() { return _system; };

			inline void referenceSystem( std::shared_ptr<Application::System> p_system ) { _system = p_system; };

			inline Application::Scene &		  getScene() { return *( _system->scene ); }
			inline const Application::Scene & getScene() const { return *( _system->scene ); }

			inline Renderer::Renderer &		  getRenderer() { return *_renderer; }
			inline const Renderer::Renderer & getRenderer() const { return *_renderer; }

			inline Application::Setting &		getSettings() { return *( _system->setting ); }
			inline const Application::Setting & getSettings() const { return *( _system->setting ); }

			inline Application::ECS::EntityDirector & getEntityDirector() { return *( _system->entityDirector ); }

		  private:
			std::shared_ptr<Application::System> _system = nullptr;
			std::unique_ptr<Renderer::Renderer>	  _renderer;

			void _handleArgs( const std::vector<std::string> & );
			void _update();
			void _stop();

			// Convenient accessors
		  public:
			static Core::ECS::Registry & MAIN_REGISTRY();
		};
	} // namespace App
} // namespace VTX

#endif
