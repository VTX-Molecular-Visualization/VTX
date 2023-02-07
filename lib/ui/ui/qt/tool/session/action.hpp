#ifndef __VTX_UI_QT_TOOL_SESSION_ACTION__
#define __VTX_UI_QT_TOOL_SESSION_ACTION__

#include "qt/application_qt.hpp"
#include "qt/dialog.hpp"
#include <old/action/base_action.hpp>
#include <old/model/molecule.hpp>
#include <old/worker/worker_manager.hpp>
#include <util/filepath.hpp>
#include <vector>

namespace VTX::UI::QT::Tool::Session::Action
{
	class Quit : public VTX::Action::BaseAction
	{
	  public:
		explicit Quit() {}
		virtual void execute() override;
	};

	class Open : public VTX::Action::BaseAction
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

		virtual void execute() override;

	  private:
		std::vector<Util::FilePath>				_paths = std::vector<Util::FilePath>();
		std::map<Util::FilePath, std::string *> _buffers;

		std::vector<Model::Molecule *> _trajectoryTargets = std::vector<Model::Molecule *>();
	};

	class Save : public VTX::Action::BaseAction
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

	class ToggleCameraController : public VTX::Action::BaseAction
	{
	  public:
		explicit ToggleCameraController() {}
		virtual void execute() override;
	};

	class ChangeCameraController : public VTX::Action::BaseAction
	{
	  public:
		explicit ChangeCameraController( const ID::VTX_ID & p_controllerId ) : _id( p_controllerId ) {}
		virtual void execute() override;

	  private:
		const ID::VTX_ID _id;
	};

	class ResetCameraController : public VTX::Action::BaseAction
	{
	  public:
		explicit ResetCameraController() {}
		virtual void execute() override;
	};
} // namespace VTX::UI::QT::Tool::Session::Action
#endif
