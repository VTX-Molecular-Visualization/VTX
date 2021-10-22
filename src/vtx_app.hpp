#ifndef __VTX_APP__
#define __VTX_APP__

#include "setting.hpp"
#include "stat.hpp"
#include <QElapsedTimer>
#include <QTimer>
#include <QtWidgets/QApplication>

namespace VTX
{
	class Setting;
	namespace Model::Representation
	{
		class RepresentationLibrary;
	}
	namespace Model::Renderer
	{
		class RenderEffectPreset;
		class RenderEffectPresetLibrary;
	} // namespace Model::Renderer
	namespace State
	{
		class StateMachine;
	}
	namespace Object3D
	{
		class Scene;
	}
	namespace IO
	{
		class ScenePathData;
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
		VTX_MASK MASK = VTX_MASK_NEED_UPDATE;

		void start( const std::vector<std::string> & );
		void goToState( const std::string &, void * const = nullptr );
		void renderScene() const;

		inline IO::ScenePathData &							  getScenePathData() { return *_pathSceneData; };
		inline const IO::ScenePathData &					  getScenePathData() const { return *_pathSceneData; };
		inline Object3D::Scene &							  getScene() { return *_scene; }
		inline const Object3D::Scene &						  getScene() const { return *_scene; }
		inline const UI::MainWindow &						  getMainWindow() const { return *_mainWindow; }
		inline UI::MainWindow &								  getMainWindow() { return *_mainWindow; }
		inline State::StateMachine &						  getStateMachine() { return *_stateMachine; }
		inline const State::StateMachine &					  getStateMachine() const { return *_stateMachine; }
		inline Setting &									  getSetting() { return _setting; }
		inline const Setting &								  getSetting() const { return _setting; }
		inline Stat &										  getStat() { return _stat; }
		inline const Stat &									  getStat() const { return _stat; }
		inline Model::Representation::RepresentationLibrary & getRepresentationLibrary()
		{
			return *_representationLibrary;
		}
		inline const Model::Representation::RepresentationLibrary & getRepresentationLibrary() const
		{
			return *_representationLibrary;
		}
		inline Model::Renderer::RenderEffectPresetLibrary & getRenderEffectLibrary() { return *_renderEffectLibrary; }
		inline const Model::Renderer::RenderEffectPresetLibrary & getRenderEffectLibrary() const
		{
			return *_renderEffectLibrary;
		}

		float getPixelRatio() const;
		bool  hasAnyModifications() const;

		bool notify( QObject * const, QEvent * const ) override;

	  private:
		QTimer *	  _timer		= nullptr;
		QElapsedTimer _elapsedTimer = QElapsedTimer();
		QElapsedTimer _tickTimer	= QElapsedTimer();
		uint		  _tickCounter	= 0u;

		Setting										   _setting				  = Setting();
		Stat										   _stat				  = Stat();
		UI::MainWindow *							   _mainWindow			  = nullptr;
		State::StateMachine *						   _stateMachine		  = nullptr;
		Object3D::Scene *							   _scene				  = nullptr;
		IO::ScenePathData *							   _pathSceneData		  = nullptr;
		Model::Representation::RepresentationLibrary * _representationLibrary = nullptr;
		Model::Renderer::RenderEffectPresetLibrary *   _renderEffectLibrary	  = nullptr;

		VTXApp();
		VTXApp( const VTXApp & ) = delete;
		VTXApp & operator=( const VTXApp & ) = delete;
		~VTXApp();

		void _initQt();
		void _handleArgs( const std::vector<std::string> & );
		void _update();
		void _stop();
	};

	Model::Renderer::RenderEffectPreset & VTX_RENDER_EFFECT();
	inline Setting &					  VTX_SETTING() { return VTXApp::get().getSetting(); }
	inline Stat &						  VTX_STAT() { return VTXApp::get().getStat(); }

} // namespace VTX

#endif
