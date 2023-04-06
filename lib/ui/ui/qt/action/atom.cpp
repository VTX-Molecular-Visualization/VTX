#include "atom.hpp"
#include "qt/application_qt.hpp"
#include "qt/state/state_machine.hpp"
#include "qt/state/visualization.hpp"

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
