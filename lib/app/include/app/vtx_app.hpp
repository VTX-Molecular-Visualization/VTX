#ifndef ___VTX_APP_VTX_APP___
#define ___VTX_APP_VTX_APP___

#include "app/application/_fwd.hpp"
// #include <QElapsedTimer>
#include <memory>
// #include <renderer/gl/opengl_renderer.hpp>
#include <string>
#include <util/exceptions.hpp>
#include <util/generic/base_static_singleton.hpp>

namespace VTX::App
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

		inline Application::Scene &		  getScene() { return *_scene; }
		inline const Application::Scene & getScene() const { return *_scene; }

		inline Application::Setting &		getSettings() { return *_setting; }
		inline const Application::Setting & getSettings() const { return *_setting; }

	  private:
		Application::Scene * _scene = nullptr;
		// std::unique_ptr<Renderer::GL::OpenGLRenderer> _renderer;
		std::unique_ptr<Application::Setting> _setting;

		void _handleArgs( const std::vector<std::string> & );
		void _update();
		void _stop();
	};
} // namespace VTX::App

#endif
