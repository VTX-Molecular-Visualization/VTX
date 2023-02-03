#include "molecule.hpp"
#include "__new_archi/ui/qt/application_qt.hpp"
#include "__new_archi/ui/qt/state/state_machine.hpp"
#include "__new_archi/ui/qt/state/visualization.hpp"

namespace VTX::UI::QT::Action::Molecule
{
	void Orient::execute()
	{
		QT_APP()
			->getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->orientCameraController( _molecule.getWorldAABB() );
	}
} // namespace VTX::UI::QT::Action::Molecule
