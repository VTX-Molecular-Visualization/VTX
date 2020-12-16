#ifndef __VTX_APP__
#define __VTX_APP__

#ifdef _MSC_VER
#pragma once
#endif

#include "setting.hpp"
#include "tool/chrono.hpp"
#include "ui/main_window.hpp"
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
		inline Object3D::Scene &	   getScene() { return *_scene; }
		inline const Object3D::Scene & getScene() const { return *_scene; }

		inline Renderer::GLSL::ProgramManager &		  getProgramManager() { return _mainWindow->getOpenGLWidget().getProgramManager(); }
		inline const Renderer::GLSL::ProgramManager & getProgramManager() const { return _mainWindow->getOpenGLWidget().getProgramManager(); }
		inline const UI::MainWindow &				  getMainWindow() const { return *_mainWindow; }
		inline UI::MainWindow &						  getMainWindow() { return *_mainWindow; }
		inline State::StateMachine &				  getStateMachine() { return *_stateMachine; }
		inline const State::StateMachine &			  getStateMachine() const { return *_stateMachine; }

		bool notify( QObject * const, QEvent * const ) override;

	  private:
		QTimer *		_timer		  = nullptr;
		QElapsedTimer * _elapsedTimer = nullptr;

		Setting				  _setting		= Setting();
		Tool::Chrono		  _chrono		= Tool::Chrono();
		UI::MainWindow *	  _mainWindow	= nullptr;
		State::StateMachine * _stateMachine = nullptr;
		Object3D::Scene *	  _scene		= nullptr;

		VTXApp();
		VTXApp( const VTXApp & ) = delete;
		VTXApp & operator=( const VTXApp & ) = delete;
		~VTXApp();

		void _initQt();
		void _update();
	};

	inline Setting & VTX_SETTING() { return VTXApp::get().getSetting(); }

} // namespace VTX

#endif
