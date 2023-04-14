#ifndef __VTX_APP__
#define __VTX_APP__

#include "old_app/define.hpp"
#include "old_app/generic/base_auto_delete.hpp"
#include "old_app/setting.hpp"
#include "old_app/spec.hpp"
#include "stat.hpp"
// #include <QElapsedTimer>
#include <util/exceptions.hpp>

namespace VTX
{
	// class Setting;
	namespace Model::Representation
	{
		class RepresentationLibrary;
	}
	namespace Model::Renderer
	{
		class RenderEffectPreset;
		class RenderEffectPresetLibrary;
	} // namespace Model::Renderer
	namespace Object3D
	{
		class Scene;
	}
	namespace IO::Struct
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
		VTX_MASK MASK = VTX_MASK_NEED_UPDATE;

		void start( const std::vector<std::string> & );
		void update();
		void goToState( const std::string &, void * const = nullptr );
		void renderScene() const;
		void stop();

		inline IO::Struct::ScenePathData &		 getScenePathData() { return *_pathSceneData; };
		inline const IO::Struct::ScenePathData & getScenePathData() const { return *_pathSceneData; };
		inline Object3D::Scene &				 getScene() { return *_scene; }
		inline const Object3D::Scene &			 getScene() const { return *_scene; }

		// TODO remove this. Must be In UI Module
		inline const UI::MainWindow & getMainWindow() const { throw NotImplementedException(); }
		inline UI::MainWindow &		  getMainWindow() { throw NotImplementedException(); }

		inline Setting &									  getSetting() { return _setting; }
		inline const Setting &								  getSetting() const { return _setting; }
		inline Stat &										  getStat() { return _stat; }
		inline const Stat &									  getStat() const { return _stat; }
		inline Spec &										  getSpec() { return _spec; }
		inline const Spec &									  getSpec() const { return _spec; }
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

		bool hasAnyModifications() const;

		void deleteAtEndOfFrame( const Generic::BaseAutoDelete * const p_object );

		// bool notify( QObject * const, QEvent * const ) override;
		void closeAllWindows();
		void exit( int p_returnCode = 0 );
		void quit();

	  private:
		// TODO Reimplement this without Qt
		// QElapsedTimer _tickTimer   = QElapsedTimer();
		// uint _tickCounter = 0u;

		Setting										   _setting				  = Setting();
		Stat										   _stat				  = Stat();
		Spec										   _spec				  = Spec();
		UI::MainWindow *							   _mainWindow			  = nullptr;
		Object3D::Scene *							   _scene				  = nullptr;
		IO::Struct::ScenePathData *					   _pathSceneData		  = nullptr;
		Model::Representation::RepresentationLibrary * _representationLibrary = nullptr;
		Model::Renderer::RenderEffectPresetLibrary *   _renderEffectLibrary	  = nullptr;

		std::vector<const Generic::BaseAutoDelete *> _deleteAtEndOfFrameObjects
			= std::vector<const Generic::BaseAutoDelete *>();

		VTXApp();
		VTXApp( const VTXApp & )			 = delete;
		VTXApp & operator=( const VTXApp & ) = delete;
		~VTXApp();

		void _initQt();
		void _handleArgs( const std::vector<std::string> & );
		void _update();
		void _stop();

		void _applyEndOfFrameDeletes();
	};

	Model::Renderer::RenderEffectPreset & VTX_RENDER_EFFECT();
	inline Setting &					  VTX_SETTING() { return VTXApp::get().getSetting(); }
	inline Stat &						  VTX_STAT() { return VTXApp::get().getStat(); }
	inline Spec &						  VTX_SPEC() { return VTXApp::get().getSpec(); }

} // namespace VTX

#endif
