#ifndef __VTX_APP__
#define __VTX_APP__

#ifdef _MSC_VER
#pragma once
#endif

#include "action/action_manager.hpp"
#include "event/event_manager.hpp"
#include "setting.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include "ui/main_window.hpp"
#include "worker/worker_manager.hpp"
#include <QTimer>
#include <QtWidgets/QApplication>

namespace VTX
{
	// Forward declaration to avoid circular dependencies and reduce compile time (maybe?)
	class Setting;
	namespace State
	{
		class StateMachine;
	}
	namespace Selection
	{
		class SelectionManager;
	}
	namespace Object3D
	{
		class Scene;
	}
	class VTXApp final : public QApplication
	{
	  public:
		inline static VTXApp & get()
		{
			static VTXApp instance;
			return instance;
		}

		void start();
		void stop();
		void goToState( const std::string &, void * const = nullptr );
		void renderScene() const { _mainWindow->getOpenGLWidget().update(); }

		inline Setting &			   getSetting() { return _setting; }
		inline const Setting &		   getSetting() const { return _setting; }
		inline Tool::Logger &		   getLogger() { return _logger; }
		inline const Tool::Logger &	   getLogger() const { return _logger; }
		inline Object3D::Scene &	   getScene() { return *_scene; }
		inline const Object3D::Scene & getScene() const { return *_scene; }

		inline Renderer::GLSL::ProgramManager &		  getProgramManager() { return _mainWindow->getOpenGLWidget().getProgramManager(); }
		inline const Renderer::GLSL::ProgramManager & getProgramManager() const { return _mainWindow->getOpenGLWidget().getProgramManager(); }
		inline const UI::MainWindow &				  getMainWindow() const { return *_mainWindow; }
		inline UI::MainWindow &						  getMainWindow() { return *_mainWindow; }
		inline State::StateMachine &				  getStateMachine() { return *_stateMachine; }
		inline const State::StateMachine &			  getStateMachine() const { return *_stateMachine; }
		inline Action::ActionManager &				  getActionManager() { return *_actionManager; }
		inline const Action::ActionManager &		  getActionManager() const { return *_actionManager; }
		inline Event::EventManager &				  getEventManager() { return *_eventManager; }
		inline const Event::EventManager &			  getEventManager() const { return *_eventManager; }
		inline Worker::WorkerManager &				  getWorkerManager() { return *_workerManager; }
		inline const Worker::WorkerManager &		  getWorkerManager() const { return *_workerManager; }
		inline Selection::SelectionManager &		  getSelectionManager() { return *_selectionManager; }
		inline const Selection::SelectionManager &	  getSelectionManager() const { return *_selectionManager; }

	  private:
		QTimer *		_timer		  = nullptr;
		QElapsedTimer * _elapsedTimer = nullptr;

		Setting						  _setting			= Setting();
		Tool::Logger				  _logger			= Tool::Logger();
		Tool::Chrono				  _chrono			= Tool::Chrono();
		UI::MainWindow *			  _mainWindow		= nullptr;
		State::StateMachine *		  _stateMachine		= nullptr;
		Object3D::Scene *			  _scene			= nullptr;
		Action::ActionManager *		  _actionManager	= nullptr;
		Event::EventManager *		  _eventManager		= nullptr;
		Worker::WorkerManager *		  _workerManager	= nullptr;
		Selection::SelectionManager * _selectionManager = nullptr;

		VTXApp();
		VTXApp( const VTXApp & ) = delete;
		VTXApp & operator=( const VTXApp & ) = delete;
		~VTXApp();

		void _update();
	};

	// TODO: check const
	inline Setting & VTX_SETTING() { return VTXApp::get().getSetting(); }
	inline void		 VTX_EVENT( VTX::Event::VTXEvent * const p_event ) { VTXApp::get().getEventManager().fireEvent( p_event ); }
	inline void		 VTX_ACTION( VTX::Action::BaseAction * const p_action, const bool p_force = false ) { VTXApp::get().getActionManager().execute( p_action, p_force ); }
	inline void		 VTX_ACTION( const std::string & p_action, const bool p_force = false ) { VTXApp::get().getActionManager().execute( p_action, p_force ); }
	// TODO: will be deleted when all workers will be threaded.
	inline void VTX_WORKER( VTX::Worker::BaseWorker * const p_worker ) { VTXApp::get().getWorkerManager().run( p_worker ); }
	inline void VTX_WORKER( VTX::Worker::BaseWorker * const p_worker, const Worker::CallbackSuccess * const p_success, const Worker::CallbackError * const p_error )
	{
		VTXApp::get().getWorkerManager().run( p_worker, p_success, p_error );
	}
	inline void VTX_DEBUG( const std::string & p_str ) { VTXApp::get().getLogger().logDebug( p_str ); }
	inline void VTX_INFO( const std::string & p_str ) { VTXApp::get().getLogger().logInfo( p_str ); }
	inline void VTX_WARNING( const std::string & p_str ) { VTXApp::get().getLogger().logWarning( p_str ); }
	inline void VTX_ERROR( const std::string & p_str ) { VTXApp::get().getLogger().logError( p_str ); }
	inline void VTX_CONSOLE( const std::string & p_str ) { std::cout << p_str << std::endl; }

	inline QOpenGLFunctions_4_5_Core & OGL() { return VTXApp::get().getMainWindow().getOpenGLWidget().getFunctions(); }
} // namespace VTX

#endif
