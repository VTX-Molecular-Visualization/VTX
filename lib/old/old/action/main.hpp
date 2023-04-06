#ifndef __VTX_ACTION_MAIN__
#define __VTX_ACTION_MAIN__

#include "base_action.hpp"
#include "io/struct/image_export.hpp"
#include "model/molecule.hpp"
#include "model/path.hpp"
#include "model/secondary_structure.hpp"
#include "model/solvent_excluded_surface.hpp"
#include "worker/snapshoter.hpp"
#include "worker/worker_manager.hpp"
#include <string>

#include <util/types.hpp>
#include <vector>

namespace VTX::Action::Main
{
	class New : public BaseAction
	{
	  public:
		virtual void execute() override;
	};

	class Quit : public BaseAction
	{
	  public:
		virtual void execute() override;
	};

	class Open : public BaseAction
	{
	  private:
		class LoadSceneClass
		{
		  public:
			LoadSceneClass( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {};

			void _loadScene();

		  private:
			std::vector<FilePath> _paths;
		};

	  public:
		explicit Open( const FilePath & p_path ) { _paths.emplace_back( p_path ); }
		explicit Open( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}
		explicit Open( const std::map<FilePath, std::string *> & p_buffers ) : _buffers( p_buffers ) {}
		explicit Open( const FilePath & p_trajectoryPath, Model::Molecule & p_target )
		{
			_trajectoryTargets.emplace_back( &p_target );
			_paths.emplace_back( p_trajectoryPath );
		}
		explicit Open( const FilePath & p_trajectoryPath, const std::vector<Model::Molecule *> & p_targets )
		{
			_trajectoryTargets.resize( p_targets.size() );
			std::copy( p_targets.begin(), p_targets.end(), _trajectoryTargets.begin() );
			_paths.emplace_back( p_trajectoryPath );
		}

		virtual void execute() override;

	  private:
		std::vector<FilePath>			  _paths = std::vector<FilePath>();
		std::map<FilePath, std::string *> _buffers;

		std::vector<Model::Molecule *> _trajectoryTargets = std::vector<Model::Molecule *>();
	};

	class OpenApi : public BaseAction
	{
	  public:
		explicit OpenApi( const std::string & p_id ) : _id( p_id ) {}

		void execute() override;

	  private:
		const std::string _id;
	};

	class Save : public BaseAction
	{
	  public:
		explicit Save( const FilePath & p_path ) : _path( p_path ), _callback( nullptr ) {}
		explicit Save( const FilePath & p_path, Worker::CallbackThread * const p_callback ) :
			_path( p_path ), _callback( p_callback )
		{
		}

		virtual void execute() override;

	  private:
		const FilePath				   _path;
		Worker::CallbackThread * const _callback;
	};

	class ImportRepresentationPreset : public BaseAction
	{
	  public:
		explicit ImportRepresentationPreset( const FilePath & p_path ) { _paths.emplace_back( p_path ); }
		explicit ImportRepresentationPreset( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}
		virtual void execute() override;

	  private:
		std::vector<FilePath> _paths = std::vector<FilePath>();
	};

	class ImportRenderEffectPreset : public BaseAction
	{
	  public:
		explicit ImportRenderEffectPreset( const FilePath & p_path ) { _paths.emplace_back( p_path ); }
		explicit ImportRenderEffectPreset( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}
		virtual void execute() override;

	  private:
		std::vector<FilePath> _paths = std::vector<FilePath>();
	};

	class ToggleCamera : public BaseAction
	{
	  public:
		explicit ToggleCamera() {}

		virtual void execute() override;
	};

	class SetCameraProjectionToPerspective : public BaseAction
	{
	  public:
		explicit SetCameraProjectionToPerspective( const bool p_perspective ) : _perspective( p_perspective ) {}

		virtual void execute() override;

	  private:
		bool _perspective;
	};

	class ToggleCameraController : public BaseAction
	{
	  public:
		explicit ToggleCameraController() {}

		virtual void execute() override;
	};

	class ChangeCameraController : public BaseAction
	{
	  public:
		explicit ChangeCameraController( const ID::VTX_ID & p_controllerId ) : _id( p_controllerId ) {}

		virtual void execute() override;

	  private:
		const ID::VTX_ID _id;
	};

	class ResetCameraController : public BaseAction
	{
	  public:
		explicit ResetCameraController() {}

		virtual void execute() override;

	  private:
	};

	class ChangeSelectionGranularity : public BaseAction
	{
	  public:
		explicit ChangeSelectionGranularity( const VTX::Selection::Granularity & p_granularity ) :
			_granularity( p_granularity )
		{
		}

		virtual void execute() override;

	  private:
		const VTX::Selection::Granularity _granularity;
	};

	class ChangePicker : public BaseAction
	{
	  public:
		explicit ChangePicker( const ID::VTX_ID & p_pickerController, const int p_mode = -1 ) :
			_pickerController( p_pickerController ), _mode( p_mode )
		{
		}

		virtual void execute() override;

	  private:
		const ID::VTX_ID & _pickerController;
		const int		   _mode;
	};

	class Snapshot : public BaseAction
	{
	  public:
		explicit Snapshot( const Worker::Snapshoter::MODE p_mode, const FilePath & p_path ) :
			Snapshot( p_mode,
					  p_path,
					  IO::Struct::ImageExport( IO::Struct::ImageExport::RESOLUTION ::Free,
											   VTX_SETTING().getSnapshotBackgroundOpacity(),
											   VTX_SETTING().getSnapshotQuality() ) )
		{
		}
		explicit Snapshot( const Worker::Snapshoter::MODE			 p_mode,
						   const FilePath &							 p_path,
						   const IO::Struct::ImageExport::RESOLUTION p_resolution ) :
			Snapshot( p_mode,
					  p_path,
					  IO::Struct::ImageExport( p_resolution,
											   VTX_SETTING().getSnapshotBackgroundOpacity(),
											   VTX_SETTING().getSnapshotQuality() ) )
		{
		}
		explicit Snapshot( const Worker::Snapshoter::MODE p_mode,
						   const FilePath &				  p_path,
						   const int					  p_width,
						   const int					  p_height ) :
			Snapshot( p_mode,
					  p_path,
					  IO::Struct::ImageExport( std::pair<const int, int>( p_width, p_height ),
											   VTX_SETTING().getSnapshotBackgroundOpacity(),
											   VTX_SETTING().getSnapshotQuality() ) )
		{
		}
		explicit Snapshot( const Worker::Snapshoter::MODE  p_mode,
						   const FilePath &				   p_path,
						   const IO::Struct::ImageExport & p_exportData ) :
			_mode( p_mode ),
			_path( p_path ), _exportData( p_exportData )
		{
		}

		virtual void execute() override
		{
			Worker::Snapshoter * worker = new Worker::Snapshoter( _mode, _path, _exportData );
			VTX_WORKER( worker );
		};

		virtual void displayUsage() override { VTX_INFO( "No parameters" ); }

	  private:
		const Worker::Snapshoter::MODE _mode;
		const FilePath				   _path;
		const IO::Struct::ImageExport  _exportData;
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

		virtual void execute() override;
		virtual void displayUsage() override { VTX_INFO( "No parameters" ); }
	};

	class CheckForUpdate : public BaseAction
	{
	  public:
		explicit CheckForUpdate( const bool p_showPopupIfNoUpdate = false ) :
			_showPopupIfNoUpdate( p_showPopupIfNoUpdate )
		{
		}

		virtual void execute() override;
		virtual void displayUsage() override { VTX_INFO( "No parameters" ); }

	  private:
		bool _showPopupIfNoUpdate;
	};

} // namespace VTX::Action::Main
#endif
