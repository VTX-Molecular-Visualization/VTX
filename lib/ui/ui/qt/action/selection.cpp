#include "selection.hpp"
#include "qt/application_qt.hpp"
#include "qt/state/state_machine.hpp"
#include "qt/state/visualization.hpp"
#include "src/object3d/helper/aabb.hpp"
#include "src/object3d/scene.hpp"
#include "src/vtx_app.hpp"

namespace VTX::UI::QT::Action::Selection
{
	void Orient::execute()
	{
		const Object3D::Helper::AABB target
			= _selection.hasMolecule() ? _selection.getAABB() : VTXApp::get().getScene().getAABB();

		QT_APP()
			->getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->orientCameraController( target );
	}
} // namespace VTX::UI::QT::Action::Selection
