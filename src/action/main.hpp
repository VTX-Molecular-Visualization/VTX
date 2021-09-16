#ifndef __VTX_ACTION_MAIN__
#define __VTX_ACTION_MAIN__

#include "action/action_manager.hpp"
#include "base_action.hpp"
#include "define.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "io/scene_path_data.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include "mvc/mvc_manager.hpp"
#include "network/network_manager.hpp"
#include "setting.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "tool/logger.hpp"
#include "ui/dialog.hpp"
#include "ui/main_window.hpp"
#include "util/filesystem.hpp"
#include "vtx_app.hpp"
#include "worker/loader.hpp"
#include "worker/render_effect_loader.hpp"
#include "worker/representation_loader.hpp"
#include "worker/saver.hpp"
#include "worker/scene_loader.hpp"
#include "worker/snapshoter.hpp"
#include "worker/worker_manager.hpp"

namespace VTX::Action::Main
{
	class New : public BaseAction
	{
	  public:
		virtual void execute() override
		{
			VTXApp::get().getScene().reset();
			VTXApp::get().getScenePathData().clearCurrentPath();
		}
	};

	class Quit : public BaseAction
	{
	  public:
		virtual void execute() override { VTXApp::get().closeAllWindows(); };
	};

	class Open : public BaseAction
	{
	  private:
		class LoadSceneClass
		{
		  public:
			LoadSceneClass( const std::vector<IO::FilePath> & p_paths ) : _paths( p_paths ) {};

			void _loadScene()
			{
				Worker::SceneLoader * sceneLoader = new Worker::SceneLoader( _paths );
				VTX_WORKER( sceneLoader );

				for ( const IO::FilePath & path : _paths )
				{
					VTXApp::get().getScenePathData().setCurrentPath( path, true );
				}
			}

		  private:
			std::vector<IO::FilePath> _paths;
		};

	  public:
		explicit Open( const IO::FilePath & p_path ) { _paths.emplace_back( p_path ); }
		explicit Open( const std::vector<IO::FilePath> & p_paths ) : _paths( p_paths ) {}
		explicit Open( const std::map<IO::FilePath, std::string *> & p_buffers ) : _buffers( p_buffers ) {}

		virtual void execute() override
		{
			bool loadScene = false;
			for ( const IO::FilePath & path : _paths )
			{
				loadScene = loadScene || path.extension() == "vtx";
			}

			if ( loadScene )
			{
				if ( _paths.empty() )
					return;

				LoadSceneClass * const sceneClass = new LoadSceneClass( _paths );

				Worker::CallbackThread callback = Worker::CallbackThread(
					[ sceneClass ]( const uint p_code )
					{
						if ( p_code )
						{
							VTXApp::get().getScene().clear();
							sceneClass->_loadScene();
						}

						delete sceneClass;
					} );
				UI::Dialog::leavingSessionDialog( callback );
			}
			else
			{
				Worker::Loader * loader = nullptr;
				if ( _paths.empty() == false )
				{
					loader = new Worker::Loader( _paths );
				}
				else if ( _buffers.empty() == false )
				{
					loader = new Worker::Loader( _buffers );
				}
				if ( loader == nullptr )
				{
					return;
				}

				Worker::CallbackThread * callback = new Worker::CallbackThread(
					[ loader ]( const uint p_code )
					{
						for ( const std::pair<IO::FilePath, Worker::Loader::Result> & pairFilResult :
							  loader->getPathsResult() )
						{
							const IO::FilePath &		   filepath = pairFilResult.first;
							const Worker::Loader::Result & result	= pairFilResult.second;

							if ( !result.state )
								continue;

							if ( result.sourceType == Worker::Loader::SOURCE_TYPE::FILE )
							{
								if ( result.molecule != nullptr )
									VTXApp::get().getScenePathData().registerLoading( result.molecule, filepath );
								VTX::Setting::enqueueNewLoadingPath( filepath );
							}
							else if ( result.sourceType == Worker::Loader::SOURCE_TYPE::BUFFER )
							{
								VTX::Setting::enqueueNewDownloadCode( filepath.filenameWithoutExtension() );
							}

							if ( result.molecule != nullptr )
							{
								result.molecule->setDisplayName( filepath.filenameWithoutExtension() );
								result.molecule->print();
								VTX_EVENT( new Event::VTXEventPtr( Event::Global::MOLECULE_CREATED, result.molecule ) );
								VTXApp::get().getScene().addMolecule( result.molecule );
							}
							else if ( result.mesh != nullptr )
							{
								VTX_EVENT( new Event::VTXEventPtr( Event::Global::MESH_CREATED, result.mesh ) );
								VTXApp::get().getScene().addMesh( result.mesh );
							}
						}

						VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
					} );

				VTX_THREAD( loader, callback );
			}
		}

	  private:
		std::vector<IO::FilePath>			  _paths = std::vector<IO::FilePath>();
		std::map<IO::FilePath, std::string *> _buffers;
	};

	class OpenApi : public BaseAction
	{
	  public:
		explicit OpenApi( const std::string & p_id ) : _id( p_id ) {}

		void execute() override { VTX_NETWORK_MANAGER().downloadMMTF( _id ); }

	  private:
		const std::string _id;
	};

	class Save : public BaseAction
	{
	  public:
		explicit Save() : _path( "" ), _callback( nullptr ) {}
		explicit Save( const IO::FilePath & p_path ) : _path( p_path ), _callback( nullptr ) {}
		explicit Save( const IO::FilePath & p_path, Worker::CallbackThread * const p_callback ) :
			_path( p_path ), _callback( p_callback )
		{
		}

		virtual void execute() override
		{
			if ( _path.empty() )
			{
				UI::Dialog::openSaveSessionDialog( _callback );
				return;
			}

			Worker::Saver * const saver = new Worker::Saver( _path );

			if ( saver == nullptr )
				return;

			VTX_THREAD( saver, _callback );

			if ( _path.extension() == "vtx" )
			{
				VTXApp::get().getScenePathData().setCurrentPath( _path, true );
				VTX_EVENT( new Event::VTXEvent( Event::Global::SCENE_SAVED ) );
			}
			else
				VTX::Setting::enqueueNewLoadingPath( _path );
		}

	  private:
		const IO::FilePath			   _path;
		Worker::CallbackThread * const _callback;
	};

	class ImportRepresentationPreset : public BaseAction
	{
	  public:
		explicit ImportRepresentationPreset( const IO::FilePath & p_path ) { _paths.emplace_back( p_path ); }
		explicit ImportRepresentationPreset( const std::vector<IO::FilePath> & p_paths ) : _paths( p_paths ) {}
		virtual void execute() override
		{
			if ( _paths.empty() )
				return;

			bool fileHasBeenImported = false;
			for ( const IO::FilePath & path : _paths )
			{
				IO::FilePath target = Util::Filesystem::getRepresentationPath( path.filename() );
				Util::Filesystem::generateUniqueFileName( target );
				if ( Util::Filesystem::copyFile( path, target ) )
				{
					Worker::RepresentationLoader * const loader = new Worker::RepresentationLoader( target );
					VTX_WORKER( loader );
				}
			}
		}

	  private:
		std::vector<IO::FilePath> _paths = std::vector<IO::FilePath>();
	};

	class ImportRenderEffectPreset : public BaseAction
	{
	  public:
		explicit ImportRenderEffectPreset( const IO::FilePath & p_path ) { _paths.emplace_back( p_path ); }
		explicit ImportRenderEffectPreset( const std::vector<IO::FilePath> & p_paths ) : _paths( p_paths ) {}
		virtual void execute() override
		{
			if ( _paths.empty() )
				return;

			bool fileHasBeenImported = false;
			for ( const IO::FilePath & path : _paths )
			{
				IO::FilePath target = Util::Filesystem::getRenderEffectPath( path.filename() );
				Util::Filesystem::generateUniqueFileName( target );
				if ( Util::Filesystem::copyFile( path, target ) )
				{
					Worker::RenderEffectPresetLoader * const loader = new Worker::RenderEffectPresetLoader( target );
					VTX_WORKER( loader );
				}
			}
		}

	  private:
		std::vector<IO::FilePath> _paths = std::vector<IO::FilePath>();
	};

	class ResetScene : public BaseAction
	{
	  public:
		virtual void execute() override { VTXApp::get().getScene().reset(); }
	};

	class ToggleCameraController : public BaseAction
	{
	  public:
		explicit ToggleCameraController() {}

		virtual void execute() override
		{
			VTXApp::get()
				.getStateMachine()
				.getItem<State::Visualization>( ID::State::VISUALIZATION )
				->toggleCameraController();
		};
	};

	class ChangeCameraController : public BaseAction
	{
	  public:
		explicit ChangeCameraController( const ID::VTX_ID & p_controllerId ) : _id( p_controllerId ) {}

		virtual void execute() override
		{
			VTXApp::get()
				.getStateMachine()
				.getItem<State::Visualization>( ID::State::VISUALIZATION )
				->setCameraController( _id );
		};

	  private:
		const ID::VTX_ID _id;
	};

	class ResetCameraController : public BaseAction
	{
	  public:
		explicit ResetCameraController() {}

		virtual void execute() override
		{
			VTXApp::get()
				.getStateMachine()
				.getItem<State::Visualization>( ID::State::VISUALIZATION )
				->resetCameraController();
		};

	  private:
	};

	class Snapshot : public BaseAction
	{
	  public:
		explicit Snapshot( const Worker::Snapshoter::MODE p_mode, const IO::FilePath & p_path ) :
			_mode( p_mode ), _path( p_path )
		{
		}

		virtual void execute() override
		{
			Worker::Snapshoter * const worker = new Worker::Snapshoter( _mode, _path, 1920, 1080 );
			VTX_WORKER( worker );
		};

		virtual void displayUsage() override { VTX_INFO( "No parameters" ); }

	  private:
		const Worker::Snapshoter::MODE _mode;
		const IO::FilePath			   _path;
	};

	class ClearConsoleInterface : public BaseAction
	{
	  public:
		explicit ClearConsoleInterface() {}

		virtual void execute() override { VTX_EVENT( new Event::VTXEvent( Event::Global::CLEAR_CONSOLE ) ); };
		virtual void displayUsage() override { VTX_INFO( "No parameters" ); }
	};

	class RestoreWindowLayout : public BaseAction
	{
	  public:
		explicit RestoreWindowLayout() {}

		virtual void execute() override { VTXApp::get().getMainWindow().restoreDefaultLayout(); };
		virtual void displayUsage() override { VTX_INFO( "No parameters" ); }
	};

} // namespace VTX::Action::Main
#endif
