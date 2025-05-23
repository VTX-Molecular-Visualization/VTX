#ifndef __VTX_APP_ACTION_IO__
#define __VTX_APP_ACTION_IO__

#include "app/core/action/base_action.hpp"
#include <util/filesystem.hpp>
#include <util/types.hpp>
#include <util/url.hpp>
#include <vector>

namespace VTX::Util::Url
{

	class SystemId;
	class UrlFull;
} // namespace VTX::Util::Url

namespace VTX::App::Action::Io
{

	class Open final : public Core::Action::BaseAction
	{
	  public:
		explicit Open( const FilePath & p_path ) { _paths.emplace_back( p_path ); }
		// explicit Open( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}
		// explicit Open( const std::map<FilePath, std::string *> & p_buffers ) : _buffers( p_buffers ) {}
		// explicit Open( const FilePath & p_trajectoryPath, Model::Chemistry::System & p_target )
		//{
		//	_trajectoryTargets.emplace_back( &p_target );
		//	_paths.emplace_back( p_trajectoryPath );
		// }
		// explicit Open( const FilePath &									 p_trajectoryPath,
		//			   const std::vector<Model::Chemistry::System *> & p_targets ) :
		//	_trajectoryTargets( p_targets )
		//{
		//	_paths.emplace_back( p_trajectoryPath );
		// }

		void execute() override;

	  private:
		std::vector<FilePath> _paths = std::vector<FilePath>();
		// std::map<FilePath, std::string *> _buffers = std::map<FilePath, std::string *>();

		// std::vector<Component::Chemistry::System *> _trajectoryTargets
		//	= std::vector<Component::Chemistry::System *>();
	};

	class DownloadSystem final : public App::Core::Action::BaseAction
	{
	  public:
		DownloadSystem( VTX::Util::Url::SystemId, FilePath );
		DownloadSystem( VTX::Util::Url::UrlFull, FilePath );
		explicit DownloadSystem( const std::string_view p_url, const FilePath & p_filename ) :
			_url( p_url ), _filename( p_filename.filename() )
		{
		}

		void execute() override;

	  private:
		const std::string_view _url;
		const FilePath		   _filename;
	};

	class SaveScene final : public Core::Action::BaseAction
	{
	  public:
		explicit SaveScene() : _path( "" ) /*, _callback( nullptr )*/ {}
		explicit SaveScene( const FilePath & p_path ) : _path( p_path ) /*, _callback( nullptr )*/ {}
		// explicit SaveScene( const FilePath & p_path, VTX::App::Old::Core::Worker::CallbackThread * const p_callback )
		// : 	_path( p_path ), _callback( p_callback )
		//{
		// }

		void execute() override;

	  private:
		const FilePath _path;
		// VTX::App::Old::Core::Worker::CallbackThread * const _callback;
	};

	class LoadSettings final : public Core::Action::BaseAction
	{
	  public:
		explicit LoadSettings();
		explicit LoadSettings( const FilePath & p_path ) : _path( p_path ) {}

		void execute() override;

	  private:
		const FilePath _path;
	};
	class SaveSettings final : public Core::Action::BaseAction
	{
	  public:
		explicit SaveSettings();
		explicit SaveSettings( const FilePath & p_path ) : _path( p_path ) {}

		void execute() override;

	  private:
		const FilePath _path;
	};
	class ReloadSettings final : public Core::Action::BaseAction
	{
	  public:
		explicit ReloadSettings() {}
		void execute() override;
	};
	class ResetSettings final : public Core::Action::BaseAction
	{
	  public:
		explicit ResetSettings() {}
		void execute() override;
	};

	class OpenScene final : public Core::Action::BaseAction
	{
	  public:
		explicit OpenScene( const FilePath & p_path ) : _path( p_path ) {}
		void execute() override;

	  private:
		FilePath _path = FilePath();
	};

} // namespace VTX::App::Action::Io
#endif
