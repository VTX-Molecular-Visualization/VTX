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
#include <util/filepath.hpp>
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
			LoadSceneClass( const std::vector<Util::FilePath> & p_paths ) : _paths( p_paths ) {};
			void _loadScene();

		  private:
			std::vector<Util::FilePath> _paths;
		};

	  public:
		explicit Open( const Util::FilePath & p_path ) { _paths.emplace_back( p_path ); }
		explicit Open( const std::vector<Util::FilePath> & p_paths ) : _paths( p_paths ) {}
		explicit Open( const std::map<Util::FilePath, std::string *> & p_buffers ) : _buffers( p_buffers ) {}
		explicit Open( const Util::FilePath & p_trajectoryPath, Model::Molecule & p_target )
		{
			_trajectoryTargets.emplace_back( &p_target );
			_paths.emplace_back( p_trajectoryPath );
		}
		explicit Open( const Util::FilePath & p_trajectoryPath, const std::vector<Model::Molecule *> & p_targets )
		{
			_trajectoryTargets.resize( p_targets.size() );
			std::copy( p_targets.begin(), p_targets.end(), _trajectoryTargets.begin() );
			_paths.emplace_back( p_trajectoryPath );
		}

		virtual void execute() override;

	  private:
		std::vector<Util::FilePath>				_paths = std::vector<Util::FilePath>();
		std::map<Util::FilePath, std::string *> _buffers;

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
		explicit Save() : _path( "" ), _callback( nullptr ) {}
		explicit Save( const Util::FilePath & p_path ) : _path( p_path ), _callback( nullptr ) {}
		explicit Save( const Util::FilePath & p_path, Worker::CallbackThread * const p_callback ) :
			_path( p_path ), _callback( p_callback )
		{
		}

		virtual void execute() override;

	  private:
		const Util::FilePath		   _path;
		Worker::CallbackThread * const _callback;
	};

	class ImportRepresentationPreset : public BaseAction
	{
	  public:
		explicit ImportRepresentationPreset( const Util::FilePath & p_path ) { _paths.emplace_back( p_path ); }
		explicit ImportRepresentationPreset( const std::vector<Util::FilePath> & p_paths ) : _paths( p_paths ) {}
		virtual void execute() override;

	  private:
		std::vector<Util::FilePath> _paths = std::vector<Util::FilePath>();
	};

	class ImportRenderEffectPreset : public BaseAction
	{
	  public:
		explicit ImportRenderEffectPreset( const Util::FilePath & p_path ) { _paths.emplace_back( p_path ); }
		explicit ImportRenderEffectPreset( const std::vector<Util::FilePath> & p_paths ) : _paths( p_paths ) {}
		virtual void execute() override;

	  private:
		std::vector<Util::FilePath> _paths = std::vector<Util::FilePath>();
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
		explicit Snapshot( const Worker::Snapshoter::MODE p_mode, const Util::FilePath & p_path );
		explicit Snapshot( const Worker::Snapshoter::MODE			 p_mode,
						   const Util::FilePath &					 p_path,
						   const IO::Struct::ImageExport::RESOLUTION p_resolution );
		explicit Snapshot( const Worker::Snapshoter::MODE p_mode,
						   const Util::FilePath &		  p_path,
						   const int					  p_width,
						   const int					  p_height );
		explicit Snapshot( const Worker::Snapshoter::MODE  p_mode,
						   const Util::FilePath &		   p_path,
						   const IO::Struct::ImageExport & p_exportData );

		virtual void execute() override;
		virtual void displayUsage() override { VTX_INFO( "No parameters" ); }

	  private:
		const Worker::Snapshoter::MODE _mode;
		const Util::FilePath		   _path;
		const IO::Struct::ImageExport  _exportData;
	};

	class ClearConsoleInterface : public BaseAction
	{
	  public:
		explicit ClearConsoleInterface() {}

		virtual void execute() override;
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
