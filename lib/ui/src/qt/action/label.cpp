#include "ui/qt/action/label.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/state/state_machine.hpp"
#include "ui/qt/state/visualization.hpp"
#include <app/core/action/base_action.hpp>
#include <app/old_app/model/label.hpp>
#include <unordered_set>
#include <vector>

namespace VTX::UI::QT::Action::Label
{
	void Orient::execute()
	{
		Object3D::Helper::AABB aabb = Object3D::Helper::AABB();

		for ( Model::Label * const label : _labels )
			aabb.extend( label->getAABB() );

		QT_APP()
			->getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->orientCameraController( aabb );
	}
} // namespace VTX::UI::QT::Action::Label
