#include "ui/qt/action/atom.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/state/state_machine.hpp"
#include "ui/qt/state/visualization.hpp"

namespace VTX::UI::QT::Action::Atom
{
	void Orient::execute()
	{
		QT_APP()
			->getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->orientCameraController( _atom.getWorldAABB() );
	}
} // namespace VTX::UI::QT::Action::Atom
