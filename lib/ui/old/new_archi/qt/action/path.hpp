#ifndef __VTX_UI_QT_ACTION_PATH__
#define __VTX_UI_QT_ACTION_PATH__

#include <app/old/core/action/base_action.hpp>
#include <app/old/component/video/path.hpp>

namespace VTX::UI::QT::Action::Path
{
	class Play : public VTX::App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Play( App::Old::Component::Video::Path * const p_path ) : _path( p_path ) {}
		virtual void execute() override;

	  private:
	 App::Old::Component::Video::Path * const _path;
	};

	/*
	class ExportVideo : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ExportVideo( App::Old::Component::Video::Path * const p_path, Worker::Snapshoter::MODE p_mode ) :
			_path( p_path ), _mode( p_mode )
		{
		}

		virtual void execute() override
		{
			State::Export::Arg arg;
			arg.mode = _mode;
			arg.path = _path;

			App::Old::VTXApp::get().goToState( ID::State::EXPORT, &arg );
		};

	  private:
		const Worker::Snapshoter::MODE _mode;
	 App::Old::Component::Video::Path * const			   _path;
	};
	*/

} // namespace VTX::UI::QT::Action::Path
#endif