#include "ui/qt/action/path.hpp"
#include "ui/id.hpp"
#include "ui/qt/application_qt.hpp"
#include <app/old_app/id.hpp>

namespace VTX::UI::QT::Action::Path
{
	void Play::execute() { QT_APP()->goToState( ID::UI_NEW::State::PLAY, _path ); };

	/*
	class ExportVideo : public Core::Action::BaseAction
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

			VTXApp::get().goToState( ID::UI_NEW::State::EXPORT, &arg );
		};

	  private:
		const Worker::Snapshoter::MODE _mode;
		Model::Path * const			   _path;
	};
	*/

} // namespace VTX::UI::QT::Action::Path
