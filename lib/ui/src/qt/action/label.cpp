#include "ui/qt/action/label.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/state/state_machine.hpp"
#include "ui/qt/state/visualization.hpp"
#include <app/old/core/action/base_action.hpp>
#include <app/old/component/object3d/label.hpp>
#include <unordered_set>
#include <vector>

namespace VTX::UI::QT::Action::Label
{
	void Orient::execute()
	{
	 App::Old::Component::Object3D::Helper::AABB aabb = App::Old::Component::Object3D::Helper::AABB();

		for ( App::Old::Component::Object3D::Label * const label : _labels )
			aabb.extend( label->getAABB() );

		QT_APP()
			->getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->orientCameraController( aabb );
	}
} // namespace VTX::UI::QT::Action::Label
