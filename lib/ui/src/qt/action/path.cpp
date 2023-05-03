#include "ui/qt/action/path.hpp"
#include "ui/id.hpp"
#include "ui/qt/application_qt.hpp"
#include <app/id.hpp>

namespace VTX::UI::QT::Action::Path
{
	void Play::execute() { QT_APP()->goToState( UI::ID::State::PLAY, _path ); };

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

			App::VTXApp::get().goToState( UI::ID::State::EXPORT, &arg );
		};

	  private:
		const Worker::Snapshoter::MODE _mode;
	 App::Component::Video::Path * const			   _path;
	};
	*/

} // namespace VTX::UI::QT::Action::Path
