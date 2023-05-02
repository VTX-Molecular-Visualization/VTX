#ifndef __VTX_APP_ACTION_MAIN__
#define __VTX_APP_ACTION_MAIN__

#include "app/action.hpp"
#include "app/application/setting.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/core/action/base_action.hpp"
#include "app/internal/io/serialization/image_export.hpp"
#include "app/internal/worker/snapshoter.hpp"
#include "app/old_app/vtx_app.hpp"
#include "app/worker.hpp"
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Action::Main
{
	class New : public App::Core::Action::BaseAction
	{
	  public:
		virtual void execute() override;
	};

	class Open : public App::Core::Action::BaseAction
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
		explicit Open( const FilePath & p_trajectoryPath, App::Component::Chemistry::Molecule & p_target )
		{
			_trajectoryTargets.emplace_back( &p_target );
			_paths.emplace_back( p_trajectoryPath );
		}
		explicit Open( const FilePath &											  p_trajectoryPath,
					   const std::vector<App::Component::Chemistry::Molecule *> & p_targets ) :
			_trajectoryTargets( p_targets )
		{
			_paths.emplace_back( p_trajectoryPath );
		}

		virtual void execute() override;

	  private:
		std::vector<FilePath>			  _paths = std::vector<FilePath>();
		std::map<FilePath, std::string *> _buffers;

		std::vector<App::Component::Chemistry::Molecule *> _trajectoryTargets
			= std::vector<App::Component::Chemistry::Molecule *>();
	};

	class OpenApi : public App::Core::Action::BaseAction
	{
	  public:
		explicit OpenApi( const std::string & p_id ) : _id( p_id ) {}

		void execute() override;

	  private:
		const std::string _id;
	};

	class Save : public App::Core::Action::BaseAction
	{
	  public:
		explicit Save() : _path( "" ), _callback( nullptr ) {}
		explicit Save( const FilePath & p_path ) : _path( p_path ), _callback( nullptr ) {}
		explicit Save( const FilePath & p_path, VTX::App::Core::Worker::CallbackThread * const p_callback ) :
			_path( p_path ), _callback( p_callback )
		{
		}

		virtual void execute() override;

	  private:
		const FilePath								   _path;
		VTX::App::Core::Worker::CallbackThread * const _callback;
	};

	class ImportRepresentationPreset : public App::Core::Action::BaseAction
	{
	  public:
		explicit ImportRepresentationPreset( const FilePath & p_path ) { _paths.emplace_back( p_path ); }
		explicit ImportRepresentationPreset( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}
		virtual void execute() override;

	  private:
		std::vector<FilePath> _paths = std::vector<FilePath>();
	};

	class ImportRenderEffectPreset : public App::Core::Action::BaseAction
	{
	  public:
		explicit ImportRenderEffectPreset( const FilePath & p_path ) { _paths.emplace_back( p_path ); }
		explicit ImportRenderEffectPreset( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}
		virtual void execute() override;

	  private:
		std::vector<FilePath> _paths = std::vector<FilePath>();
	};

	class ToggleCamera : public App::Core::Action::BaseAction
	{
	  public:
		explicit ToggleCamera() {}

		virtual void execute() override;
	};

	class SetCameraProjectionToPerspective : public App::Core::Action::BaseAction
	{
	  public:
		explicit SetCameraProjectionToPerspective( const bool p_perspective ) : _perspective( p_perspective ) {}

		virtual void execute() override;

	  private:
		bool _perspective;
	};

	class Snapshot : public App::Core::Action::BaseAction
	{
	  public:
		explicit Snapshot( const Worker::Snapshoter::MODE p_mode, const FilePath & p_path ) :
			Snapshot( p_mode,
					  p_path,
					  App::Internal::IO::Serialization::ImageExport(
						  App::Internal::IO::Serialization::ImageExport::RESOLUTION::Free,
						  VTX_SETTING().getSnapshotBackgroundOpacity(),
						  VTX_SETTING().getSnapshotQuality() ) )
		{
		}
		explicit Snapshot( const Worker::Snapshoter::MODE								   p_mode,
						   const FilePath &												   p_path,
						   const App::Internal::IO::Serialization::ImageExport::RESOLUTION p_resolution ) :
			Snapshot( p_mode,
					  p_path,
					  App::Internal::IO::Serialization::ImageExport( p_resolution,
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
					  App::Internal::IO::Serialization::ImageExport( std::pair<const int, int>( p_width, p_height ),
																	 VTX_SETTING().getSnapshotBackgroundOpacity(),
																	 VTX_SETTING().getSnapshotQuality() ) )
		{
		}
		explicit Snapshot( const Worker::Snapshoter::MODE						 p_mode,
						   const FilePath &										 p_path,
						   const App::Internal::IO::Serialization::ImageExport & p_exportData ) :
			_mode( p_mode ),
			_path( p_path ), _exportData( p_exportData )
		{
		}

		virtual void execute() override;

		virtual void displayUsage() override { VTX_INFO( "No parameters" ); }

	  private:
		const Worker::Snapshoter::MODE						_mode;
		const FilePath										_path;
		const App::Internal::IO::Serialization::ImageExport _exportData;
	};

} // namespace VTX::App::Action::Main
#endif
