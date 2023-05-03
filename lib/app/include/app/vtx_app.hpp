#ifndef __VTX_APP_VTX_APP__
#define __VTX_APP_VTX_APP__

#include "app/application/_fwd.hpp"
#include "app/application/setting.hpp"
#include "app/internal/monitoring/stat.hpp"
#include "app/old_app/generic/base_auto_delete.hpp"
#include "app/render/define.hpp"
#include "app/render/spec.hpp"
// #include <QElapsedTimer>
#include <string>
#include <util/exceptions.hpp>
#include <vector>

namespace VTX
{
	namespace App
	{
		namespace Internal::IO::Serialization
		{
			class ScenePathData;
		}

		class VTXApp // final
		{
		  public:
			inline static VTXApp & get()
			{
				static VTXApp instance;
				return instance;
			}
			Render::VTX_MASK MASK = Render::VTX_MASK_NEED_UPDATE;

			void start( const std::vector<std::string> & );
			void update();
			void goToState( const std::string &, void * const = nullptr );
			void renderScene() const;
			void stop();

			inline Internal::IO::Serialization::ScenePathData &		  getScenePathData() { return *_pathSceneData; };
			inline const Internal::IO::Serialization::ScenePathData & getScenePathData() const
			{
				return *_pathSceneData;
			};
			inline Application::Scene &		  getScene() { return *_scene; }
			inline const Application::Scene & getScene() const { return *_scene; }

			inline Application::Setting &								getSetting() { return _setting; }
			inline const Application::Setting &							getSetting() const { return _setting; }
			inline Internal::Monitoring::Stat &							getStat() { return _stat; }
			inline const Internal::Monitoring::Stat &					getStat() const { return _stat; }
			inline Render::Spec &										getSpec() { return _spec; }
			inline const Render::Spec &									getSpec() const { return _spec; }
			inline Application::Representation::RepresentationLibrary & getRepresentationLibrary()
			{
				return *_representationLibrary;
			}
			inline const Application::Representation::RepresentationLibrary & getRepresentationLibrary() const
			{
				return *_representationLibrary;
			}
			inline Application::RenderEffect::RenderEffectLibrary & getRenderEffectLibrary()
			{
				return *_renderEffectLibrary;
			}
			inline const Application::RenderEffect::RenderEffectLibrary & getRenderEffectLibrary() const
			{
				return *_renderEffectLibrary;
			}

			bool hasAnyModifications() const;

			void deleteAtEndOfFrame( const Generic::BaseAutoDelete * const p_object );

		  protected:
			VTXApp();
			VTXApp( const VTXApp & )			 = delete;
			VTXApp & operator=( const VTXApp & ) = delete;
			~VTXApp();

		  private:
			// TODO Reimplement this without Qt
			// QElapsedTimer _tickTimer   = QElapsedTimer();
			// uint _tickCounter = 0u;

			Application::Setting								 _setting				= Application::Setting();
			Internal::Monitoring::Stat							 _stat					= Internal::Monitoring::Stat();
			Render::Spec										 _spec					= Render::Spec();
			Application::Scene *								 _scene					= nullptr;
			Internal::IO::Serialization::ScenePathData *		 _pathSceneData			= nullptr;
			Application::Representation::RepresentationLibrary * _representationLibrary = nullptr;
			Application::RenderEffect::RenderEffectLibrary *	 _renderEffectLibrary	= nullptr;

			std::vector<const Generic::BaseAutoDelete *> _deleteAtEndOfFrameObjects
				= std::vector<const Generic::BaseAutoDelete *>();

			void _initQt();
			void _handleArgs( const std::vector<std::string> & );
			void _update();
			void _stop();

			void _applyEndOfFrameDeletes();
		};
	} // namespace App

	App::Application::RenderEffect::RenderEffectPreset & VTX_RENDER_EFFECT();
	inline App::Application::Setting &					 VTX_SETTING() { return App::VTXApp::get().getSetting(); }
	inline App::Internal::Monitoring::Stat &			 VTX_STAT() { return App::VTXApp::get().getStat(); }
	inline App::Render::Spec &							 VTX_SPEC() { return App::VTXApp::get().getSpec(); }

} // namespace VTX

#endif
