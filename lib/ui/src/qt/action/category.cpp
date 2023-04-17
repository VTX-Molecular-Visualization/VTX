#include "ui/qt/action/category.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/state/state_machine.hpp"
#include "ui/qt/state/visualization.hpp"

namespace VTX::UI::QT::Action::Category
{
	void Orient::execute()
	{
		QT_APP()
			->getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->orientCameraController( _category.getWorldAABB() );
	}

} // namespace VTX::UI::QT::Action::Category
