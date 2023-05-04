#ifndef __VTX_UI_APP__
#define __VTX_UI_APP__

#include "ui/old_ui/state/state_machine.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include <QApplication>
#include <app/vtx_app.hpp>

namespace VTX::UI
{
	class VTXApp : public ::VTX::App::VTXApp
	{
	  public:
		inline static VTXApp & get()
		{
			static VTXApp instance;
			return instance;
		}

		inline const UI::MainWindow &	   getMainWindow() const { throw NotImplementedException(); }
		inline UI::MainWindow &			   getMainWindow() { throw NotImplementedException(); }
		inline State::StateMachine &	   getStateMachine() { throw NotImplementedException(); }
		inline const State::StateMachine & getStateMachine() const { throw NotImplementedException(); }

		QInputMethod * inputMethod() const { return QApplication::inputMethod(); }

		void closeAllWindows();
		void exit( int p_returnCode = 0 );
		void quit();

	  private:
		VTXApp() : ::VTX::App::VTXApp() {};
		VTXApp( const VTXApp & )			 = delete;
		VTXApp & operator=( const VTXApp & ) = delete;
		~VTXApp() {}
	};
} // namespace VTX::UI

//
//// #include <app/application/setting.hpp>
// #include "state/state_machine.hpp"
// #include <QElapsedTimer>
// #include <QInputMethod>
// #include <QTimer>
// #include <app/internal/define.hpp>
// #include <app/component/generic/base_auto_delete.hpp>
// #include <app/render/spec.hpp>
// #include <app/old_app/stat.hpp>
// #include <util/exceptions.hpp>
//
// namespace VTX
//{
//	// class Setting;
//	namespace Model::Representation
//	{
//		class RepresentationLibrary;
//	}
//	namespace Model::Renderer
//	{
//		class RenderEffectPreset;
//		class RenderEffectLibrary;
//	} // namespace Model::Renderer
//	namespace Object3D
//	{
//		class Scene;
//	}
//	namespace IO::Struct
//	{
//		class ScenePathData;
//	}
//	namespace UI
//	{
//		class MainWindow;
//	}
//	class VTXApp final
//	{
//	  public:
//		inline static VTXApp & get()
//		{
//			static VTXApp instance;
//			return instance;
//		}
//		VTX_MASK MASK = Render::VTX_MASK_NEED_UPDATE;
//
//		void start( const std::vector<std::string> & );
//		void update();
//		void goToState( const std::string &, void * const = nullptr );
//		void renderScene() const;
//		void stop();
//
//		inline App::Internal::IO::Serialization::ScenePathData &		 getScenePathData() { return *_pathSceneData; };
//		inline const App::Internal::IO::Serialization::ScenePathData & getScenePathData() const { return
//*_pathSceneData;
//}; 		inline App::Application::Scene &				 getScene() { return *_scene; } 		inline const
// App::Application::Scene &			 getScene() const { return *_scene; }
//
//		// TODO remove this. Must be In UI Module
//		inline const UI::MainWindow &	   getMainWindow() const { throw NotImplementedException(); }
//		inline UI::MainWindow &			   getMainWindow() { throw NotImplementedException(); }
//		inline State::StateMachine &	   getStateMachine() { throw NotImplementedException(); }
//		inline const State::StateMachine & getStateMachine() const { throw NotImplementedException(); }
//
//		// inline Setting &									  getSetting() { return _setting; }
//		// inline const Setting &								  getSetting() const { return _setting; }
//		// inline Stat &										  getStat() { return _stat; }
//		// inline const Stat &									  getStat() const { return _stat; }
//		// inline Spec &										  getSpec() { return _spec; }
//		// inline const Spec &									  getSpec() const { return _spec; }
//		inline App::Application::Representation::RepresentationLibrary & getRepresentationLibrary()
//		{
//			return *_representationLibrary;
//		}
//		inline const App::Application::Representation::RepresentationLibrary & getRepresentationLibrary() const
//		{
//			return *_representationLibrary;
//		}
//		inline App::Application::RenderEffect::RenderEffectLibrary & getRenderEffectLibrary() { return
//*_renderEffectLibrary; } 		inline const App::Application::RenderEffect::RenderEffectLibrary &
// getRenderEffectLibrary() const
//		{
//			return *_renderEffectLibrary;
//		}
//
//		bool hasAnyModifications() const;
//
//		void deleteAtEndOfFrame( const Generic::BaseAutoDelete * const p_object );
//
//		// bool notify( QObject * const, QEvent * const ) override;
//		void		   closeAllWindows();
//		QInputMethod * inputMethod();
//		void		   exit( int p_returnCode = 0 );
//		void		   quit();
//
//	  private:
//		QTimer *	  _timer		= nullptr;
//		QElapsedTimer _elapsedTimer = QElapsedTimer();
//		QElapsedTimer _tickTimer	= QElapsedTimer();
//		uint		  _tickCounter	= 0u;
//
//		// Setting										   _setting				  = Setting();
//		// Stat										   _stat				  = Stat();
//		// Spec										   _spec				  = Spec();
//		UI::MainWindow *							   _mainWindow			  = nullptr;
//		State::StateMachine *						   _stateMachine		  = nullptr;
//		App::Application::Scene *							   _scene				  = nullptr;
//		App::Internal::IO::Serialization::ScenePathData *					   _pathSceneData		  = nullptr;
//		App::Application::Representation::RepresentationLibrary * _representationLibrary = nullptr;
//		App::Application::RenderEffect::RenderEffectLibrary *   _renderEffectLibrary	  = nullptr;
//
//		std::vector<const Generic::BaseAutoDelete *> _deleteAtEndOfFrameObjects
//			= std::vector<const Generic::BaseAutoDelete *>();
//
//		VTXApp();
//		VTXApp( const VTXApp & )			 = delete;
//		VTXApp & operator=( const VTXApp & ) = delete;
//		~VTXApp();
//
//		void _initQt();
//		void _handleArgs( const std::vector<std::string> & );
//		void _update();
//		void _stop();
//
//		void _applyEndOfFrameDeletes();
//	};
//
//	// App::Application::RenderEffect::RenderEffectPreset & VTX_RENDER_EFFECT();
//	//  inline Setting &					  VTX_SETTING() { return App::VTXApp::get().getSetting(); }
//	//  inline Stat & VTX_STAT() { return App::VTXApp::get().getStat(); }
//	//  inline Spec & VTX_SPEC() { return App::VTXApp::get().getSpec(); }
//
// } // namespace VTX
//
#endif
