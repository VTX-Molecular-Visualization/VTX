#ifndef __VTX_APP__
#define __VTX_APP__

#include "app/application/_fwd.hpp"
#include "app/application/setting.hpp"
#include "app/old_app/generic/base_auto_delete.hpp"
#include "app/render/define.hpp"
#include "app/render/spec.hpp"
#include "stat.hpp"
// #include <QElapsedTimer>
#include <util/exceptions.hpp>

namespace VTX
{
	namespace App::Internal::IO::Serialization
	{
		class ScenePathData;
	}
	namespace UI
	{
		class MainWindow;
	}
	class VTXApp // final
	{
	  public:
		inline static VTXApp & get()
		{
			static VTXApp instance;
			return instance;
		}
		App::Render::VTX_MASK MASK = App::Render::VTX_MASK_NEED_UPDATE;

		void start( const std::vector<std::string> & );
		void update();
		void goToState( const std::string &, void * const = nullptr );
		void renderScene() const;
		void stop();

		inline App::Internal::IO::Serialization::ScenePathData &	   getScenePathData() { return *_pathSceneData; };
		inline const App::Internal::IO::Serialization::ScenePathData & getScenePathData() const
		{
			return *_pathSceneData;
		};
		inline App::Application::Scene &	   getScene() { return *_scene; }
		inline const App::Application::Scene & getScene() const { return *_scene; }

		// TODO remove this. Must be In UI Module
		inline const UI::MainWindow & getMainWindow() const { throw NotImplementedException(); }
		inline UI::MainWindow &		  getMainWindow() { throw NotImplementedException(); }

		inline App::Application::Setting &								 getSetting() { return _setting; }
		inline const App::Application::Setting &						 getSetting() const { return _setting; }
		inline Stat &													 getStat() { return _stat; }
		inline const Stat &												 getStat() const { return _stat; }
		inline App::Render::Spec &										 getSpec() { return _spec; }
		inline const App::Render::Spec &								 getSpec() const { return _spec; }
		inline App::Application::Representation::RepresentationLibrary & getRepresentationLibrary()
		{
			return *_representationLibrary;
		}
		inline const App::Application::Representation::RepresentationLibrary & getRepresentationLibrary() const
		{
			return *_representationLibrary;
		}
		inline App::Application::RenderEffect::RenderEffectLibrary & getRenderEffectLibrary()
		{
			return *_renderEffectLibrary;
		}
		inline const App::Application::RenderEffect::RenderEffectLibrary & getRenderEffectLibrary() const
		{
			return *_renderEffectLibrary;
		}

		bool hasAnyModifications() const;

		void deleteAtEndOfFrame( const Generic::BaseAutoDelete * const p_object );

		// bool notify( QObject * const, QEvent * const ) override;
		void closeAllWindows();
		void exit( int p_returnCode = 0 );
		void quit();

	  protected:
		VTXApp();
		VTXApp( const VTXApp & )			 = delete;
		VTXApp & operator=( const VTXApp & ) = delete;
		~VTXApp();

	  private:
		// TODO Reimplement this without Qt
		// QElapsedTimer _tickTimer   = QElapsedTimer();
		// uint _tickCounter = 0u;

		App::Application::Setting								  _setting				 = App::Application::Setting();
		Stat													  _stat					 = Stat();
		App::Render::Spec										  _spec					 = App::Render::Spec();
		UI::MainWindow *										  _mainWindow			 = nullptr;
		App::Application::Scene *								  _scene				 = nullptr;
		App::Internal::IO::Serialization::ScenePathData *		  _pathSceneData		 = nullptr;
		App::Application::Representation::RepresentationLibrary * _representationLibrary = nullptr;
		App::Application::RenderEffect::RenderEffectLibrary *	  _renderEffectLibrary	 = nullptr;

		std::vector<const Generic::BaseAutoDelete *> _deleteAtEndOfFrameObjects
			= std::vector<const Generic::BaseAutoDelete *>();

		void _initQt();
		void _handleArgs( const std::vector<std::string> & );
		void _update();
		void _stop();

		void _applyEndOfFrameDeletes();
	};

	App::Application::RenderEffect::RenderEffectPreset & VTX_RENDER_EFFECT();
	inline App::Application::Setting &					 VTX_SETTING() { return VTXApp::get().getSetting(); }
	inline Stat &										 VTX_STAT() { return VTXApp::get().getStat(); }
	inline App::Render::Spec &							 VTX_SPEC() { return VTXApp::get().getSpec(); }

} // namespace VTX

#endif
