#include "selection.hpp"
#include "__new_archi/ui/qt/application_qt.hpp"
#include "__new_archi/ui/qt/state/state_machine.hpp"
#include "__new_archi/ui/qt/state/visualization.hpp"
#include "object3d/helper/aabb.hpp"
#include "object3d/scene.hpp"
#include "vtx_app.hpp"

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
