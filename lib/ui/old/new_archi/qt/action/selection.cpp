#include "ui/qt/action/selection.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/state/state_machine.hpp"
#include "ui/qt/state/visualization.hpp"
#include <app/old/component/object3d/helper/aabb.hpp>
#include <app/old/application/scene.hpp>
#include <app/old/vtx_app.hpp>

namespace VTX::UI::QT::Action::Selection
{
	void Orient::execute()
	{
		const App::Old::Component::Object3D::Helper::AABB target
			= _selection.hasMolecule() ? _selection.getAABB() : App::Old::VTXApp::get().getScene().getAABB();

		QT_APP()
			->getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->orientCameraController( target );
	}
} // namespace VTX::UI::QT::Action::Selection
