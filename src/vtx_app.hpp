#ifndef __VTX_APP__
#define __VTX_APP__

#ifdef _MSC_VER
#pragma once
#endif

#include "setting.hpp"
#include "stat.hpp"
#include <QElapsedTimer>
#include <QTimer>
#include <QtWidgets/QApplication>

namespace VTX
{
	class Setting;
	namespace State
	{
		class StateMachine;
	}
	namespace Object3D
	{
		class Scene;
	}
	namespace UI
	{
		class MainWindow;
	}
	class VTXApp final : public QApplication
	{
	  public:
		inline static VTXApp & get()
		{
			static VTXApp instance;
			return instance;
		}
		VTX_MASK MASK = 0xFF;

		void start();
		void stop();
		void goToState( const std::string &, void * const = nullptr );
		void renderScene() const;

		inline Setting &				   getSetting() { return _setting; }
		inline const Setting &			   getSetting() const { return _setting; }
		inline Stat &					   getStat() { return _stat; }
		inline const Stat &				   getStat() const { return _stat; }
		inline Object3D::Scene &		   getScene() { return *_scene; }
		inline const Object3D::Scene &	   getScene() const { return *_scene; }
		inline const UI::MainWindow &	   getMainWindow() const { return *_mainWindow; }
		inline UI::MainWindow &			   getMainWindow() { return *_mainWindow; }
		inline State::StateMachine &	   getStateMachine() { return *_stateMachine; }
		inline const State::StateMachine & getStateMachine() const { return *_stateMachine; }

		bool notify( QObject * const, QEvent * const ) override;

	  private:
		QTimer *		_timer		  = nullptr;
		QElapsedTimer * _elapsedTimer = nullptr;

		Setting				  _setting		= Setting();
		Stat				  _stat			= Stat();
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
	inline Stat &	 VTX_STAT() { return VTXApp::get().getStat(); }

} // namespace VTX

#endif
