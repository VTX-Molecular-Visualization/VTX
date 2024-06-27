#ifndef __VTX_APP_ACTION_APPLICATION__
#define __VTX_APP_ACTION_APPLICATION__

#include "app/core/action/base_action.hpp"
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Action::Application
{
	class NewScene final : public Core::Action::BaseAction
	{
	  public:
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
	class ClearScene final : public Core::Action::BaseAction
	{
	  public:
		void execute() override;
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

	class Open final : public Core::Action::BaseAction
	{
	  public:
		explicit Open( const FilePath & p_path ) { _paths.emplace_back( p_path ); }
		// explicit Open( const std::vector<FilePath> & p_paths ) : _paths( p_paths ) {}
		// explicit Open( const std::map<FilePath, std::string *> & p_buffers ) : _buffers( p_buffers ) {}
		// explicit Open( const FilePath & p_trajectoryPath, Model::Chemistry::Molecule & p_target )
		//{
		//	_trajectoryTargets.emplace_back( &p_target );
		//	_paths.emplace_back( p_trajectoryPath );
		// }
		// explicit Open( const FilePath &									 p_trajectoryPath,
		//			   const std::vector<Model::Chemistry::Molecule *> & p_targets ) :
		//	_trajectoryTargets( p_targets )
		//{
		//	_paths.emplace_back( p_trajectoryPath );
		// }

		void execute() override;

	  private:
		std::vector<FilePath> _paths = std::vector<FilePath>();
		// std::map<FilePath, std::string *> _buffers = std::map<FilePath, std::string *>();

		// std::vector<Component::Chemistry::Molecule *> _trajectoryTargets
		//	= std::vector<Component::Chemistry::Molecule *>();
	};

	class Quit final : public Core::Action::BaseAction
	{
	  public:
		Quit() {};
		void execute() override;
	};

	class RunScript : public Core::Action::BaseAction
	{
	  public:
		explicit RunScript( const FilePath & p_path ) : _path( p_path ) {}
		void execute();

	  private:
		FilePath _path;
	};
} // namespace VTX::App::Action::Application
#endif
