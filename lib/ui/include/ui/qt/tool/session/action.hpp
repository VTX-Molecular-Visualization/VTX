#ifndef __VTX_UI_QT_TOOL_SESSION_ACTION__
#define __VTX_UI_QT_TOOL_SESSION_ACTION__

#include "ui/qt/application_qt.hpp"
#include "ui/qt/dialog.hpp"
#include <app/core/action/base_action.hpp>
#include <app/core/worker/callback.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::UI::QT::Tool::Session::Action
{
	class Open : public VTX::App::Core::Action::BaseAction
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

		virtual void execute() override;

	  private:
		void _callEffectiveOpenAction();

	  private:
		std::vector<FilePath>			  _paths = std::vector<FilePath>();
		std::map<FilePath, std::string *> _buffers;

		std::vector<App::Component::Chemistry::Molecule *> _trajectoryTargets = std::vector<App::Component::Chemistry::Molecule *>();
	};

	class Save : public VTX::App::Core::Action::BaseAction
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

	class ToggleCameraController : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit ToggleCameraController() {}
		virtual void execute() override;
	};

	class ChangeCameraController : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeCameraController( const App::VTX_ID & p_controllerId ) : _id( p_controllerId ) {}
		virtual void execute() override;

	  private:
		const App::VTX_ID _id;
	};

	class ResetCameraController : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit ResetCameraController() {}
		virtual void execute() override;
	};
} // namespace VTX::UI::QT::Tool::Session::Action
#endif
