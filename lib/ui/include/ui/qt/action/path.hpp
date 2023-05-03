#ifndef __VTX_UI_QT_ACTION_PATH__
#define __VTX_UI_QT_ACTION_PATH__

#include <app/core/action/base_action.hpp>
#include <app/component/video/path.hpp>

namespace VTX::UI::QT::Action::Path
{
	class Play : public VTX::App::Core::Action::BaseAction
	{
	  public:
		explicit Play( App::Component::Video::Path * const p_path ) : _path( p_path ) {}
		virtual void execute() override;

	  private:
	 App::Component::Video::Path * const _path;
	};

	/*
	class ExportVideo : public App::Core::Action::BaseAction
	{
	  public:
		explicit ExportVideo( App::Component::Video::Path * const p_path, Worker::Snapshoter::MODE p_mode ) :
			_path( p_path ), _mode( p_mode )
		{
		}

		virtual void execute() override
		{
			State::Export::Arg arg;
			arg.mode = _mode;
			arg.path = _path;

			App::VTXApp::get().goToState( ID::State::EXPORT, &arg );
		};

	  private:
		const Worker::Snapshoter::MODE _mode;
	 App::Component::Video::Path * const			   _path;
	};
	*/

} // namespace VTX::UI::QT::Action::Path
#endif
