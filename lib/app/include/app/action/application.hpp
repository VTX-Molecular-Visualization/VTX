#ifndef __VTX_APP_ACTION_APPLICATION__
#define __VTX_APP_ACTION_APPLICATION__

#include "app/core/action/base_action.hpp"
#include <util/types.hpp>

namespace VTX::App::Action::Application
{
	class NewScene final : public App::Core::Action::BaseAction
	{
	  public:
		void execute() override;
	};
	class OpenScene final : public App::Core::Action::BaseAction
	{
	  public:
		explicit OpenScene( const FilePath & p_path ) : _path( p_path ) {}
		void execute() override;

	  private:
		FilePath _path = FilePath();
	};
	class SaveScene final : public App::Core::Action::BaseAction
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
} // namespace VTX::App::Action::Application
#endif
