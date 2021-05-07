#ifndef __VTX_ACTION_MAIN__
#define __VTX_ACTION_MAIN__

#ifdef _MSC_VER
#pragma once
#endif

#include "action/action_manager.hpp"
#include "base_action.hpp"
#include "define.hpp"
#include "model/path.hpp"
#include "mvc/mvc_manager.hpp"
#include "network/network_manager.hpp"
#include "setting.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "tool/logger.hpp"
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
		virtual void execute() override { VTXApp::get().getScene().reset(); }
	};

	class Quit : public BaseAction
	{
	  public:
		virtual void execute() override { VTXApp::get().stop(); };
	};

	class Open : public BaseAction
	{
	  public:
		explicit Open( FilePath * const p_path ) { _paths.emplace_back( p_path ); }
		explicit Open( const std::vector<FilePath *> & p_paths ) : _paths( p_paths ) {}
		explicit Open( const std::map<FilePath *, std::string *> & p_buffers ) : _buffers( p_buffers ) {}

		virtual void execute() override
		{
			bool loadScene = false;
			for ( const FilePath * const path : _paths )
			{
				loadScene = loadScene || path->extension() == ".vtx";
			}

			if ( loadScene )
			{
				if ( _paths.empty() )
					return;

				Worker::SceneLoader * sceneLoader = new Worker::SceneLoader( _paths );
				VTX_WORKER( sceneLoader );

				for ( FilePath * const path : _paths )
				{
					VTXApp::get().setCurrentPath( *path, true );
					delete path;
				}
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

				Worker::Callback * callback = new Worker::Callback(
					[ loader ]( const uint p_code )
					{
						for ( Model::Molecule * const molecule : loader->getMolecules() )
						{
							molecule->print();
							VTX_EVENT( new Event::VTXEventPtr( Event::Global::MOLECULE_CREATED, molecule ) );
							VTXApp::get().getScene().addMolecule( molecule );
						}
						for ( Model::MeshTriangle * const mesh : loader->getMeshes() )
						{
							VTX_EVENT( new Event::VTXEventPtr( Event::Global::MESH_CREATED, mesh ) );
							VTXApp::get().getScene().addMesh( mesh );
						}

						for ( std::pair<const FilePath *, bool> pairPathState : loader->getPathsState() )
						{
							if ( pairPathState.second )
								VTX::Setting::enqueueNewLoadingPath( *pairPathState.first );
						}

						VTXApp::get().MASK |= VTX_MASK_NEED_UPDATE;
					} );

				VTX_WORKER( loader, callback );
			}
		}

	  private:
		std::vector<FilePath *>				_paths = std::vector<FilePath *>();
		std::map<FilePath *, std::string *> _buffers;
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
		explicit Save( FilePath * p_path ) : _path( p_path ) {}

		virtual void execute() override
		{
			Worker::Saver * saver = nullptr;
			if ( _path->empty() == false )
			{
				saver = new Worker::Saver( _path );
			}
			if ( saver == nullptr )
			{
				return;
			}

			Worker::Callback * callback = new Worker::Callback( [ saver ]( const uint p_code ) {} );
			VTX_WORKER( saver, callback );

			if ( _path->extension() == ".vtx" )
				VTXApp::get().setCurrentPath( *_path, true );
			else
				VTX::Setting::enqueueNewLoadingPath( *_path );
		}

	  private:
		FilePath * _path;
	};

	class ImportRepresentationPreset : public BaseAction
	{
	  public:
		explicit ImportRepresentationPreset( FilePath * const p_path ) { _paths.emplace_back( p_path ); }
		explicit ImportRepresentationPreset( const std::vector<FilePath *> & p_paths ) : _paths( p_paths ) {}
		virtual void execute() override
		{
			if ( _paths.empty() )
				return;

			bool fileHasBeenImported = false;
			for ( FilePath * const path : _paths )
			{
				FilePath target = Util::Filesystem::getRepresentationPath( path->filename().string() );
				if ( Util::Filesystem::copyFile( *path, target, true ) )
				{
					Worker::RepresentationLoader * const loader = new Worker::RepresentationLoader( target );
					VTX_WORKER( loader );
				}

				delete path;
			}
		}

	  private:
		std::vector<FilePath *> _paths = std::vector<FilePath *>();
	};

	class ImportRenderEffectPreset : public BaseAction
	{
	  public:
		explicit ImportRenderEffectPreset( FilePath * const p_path ) { _paths.emplace_back( p_path ); }
		explicit ImportRenderEffectPreset( const std::vector<FilePath *> & p_paths ) : _paths( p_paths ) {}
		virtual void execute() override
		{
			if ( _paths.empty() )
				return;

			bool fileHasBeenImported = false;
			for ( FilePath * const path : _paths )
			{
				FilePath target = Util::Filesystem::getRenderEffectPath( path->filename().string() );
				if ( Util::Filesystem::copyFile( *path, target, true ) )
				{
					Worker::RenderEffectPresetLoader * const loader = new Worker::RenderEffectPresetLoader( target );
					VTX_WORKER( loader );
				}

				delete path;
			}
		}

	  private:
		std::vector<FilePath *> _paths = std::vector<FilePath *>();
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
		explicit Snapshot( const Worker::Snapshoter::MODE p_mode, const FilePath & p_path ) :
			_mode( p_mode ), _path( p_path )
		{
		}

		virtual void execute() override
		{
			Worker::Snapshoter * const worker = new Worker::Snapshoter( _mode, _path );
			VTX_WORKER( worker );
		};

		virtual void displayUsage() override { VTX_INFO( "No parameters" ); }

	  private:
		const Worker::Snapshoter::MODE _mode;
		const FilePath				   _path;
	};

} // namespace VTX::Action::Main
#endif
