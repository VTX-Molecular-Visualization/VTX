#include "ui/qt/action/selection.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/state/state_machine.hpp"
#include "ui/qt/state/visualization.hpp"
#include <app/component/object3d/helper/aabb.hpp>
#include <app/application/scene.hpp>
#include <app/vtx_app.hpp>

namespace VTX::UI::QT::Action::Selection
{
	void Orient::execute()
	{
		const App::Component::Object3D::Helper::AABB target
			= _selection.hasMolecule() ? _selection.getAABB() : App::VTXApp::get().getScene().getAABB();

		QT_APP()
			->getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->orientCameraController( target );
	}
} // namespace VTX::UI::QT::Action::Selection
