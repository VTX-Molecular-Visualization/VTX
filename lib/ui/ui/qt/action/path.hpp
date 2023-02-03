#ifndef __VTX_UI_QT_ACTION_PATH__
#define __VTX_UI_QT_ACTION_PATH__

#include "src/action/base_action.hpp"
#include "src/model/path.hpp"

namespace VTX::UI::QT::Action::Path
{
	class Play : public VTX::Action::BaseAction
	{
	  public:
		explicit Play( Model::Path * const p_path ) : _path( p_path ) {}
		virtual void execute() override;

	  private:
		Model::Path * const _path;
	};

	/*
	class ExportVideo : public BaseAction
	{
	  public:
		explicit ExportVideo( Model::Path * const p_path, Worker::Snapshoter::MODE p_mode ) :
			_path( p_path ), _mode( p_mode )
		{
		}

		virtual void execute() override
		{
			State::Export::Arg arg;
			arg.mode = _mode;
			arg.path = _path;

			VTXApp::get().goToState( ID::State::EXPORT, &arg );
		};

	  private:
		const Worker::Snapshoter::MODE _mode;
		Model::Path * const			   _path;
	};
	*/

} // namespace VTX::UI::QT::Action::Path
#endif
