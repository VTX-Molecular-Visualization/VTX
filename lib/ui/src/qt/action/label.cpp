#include "ui/qt/action/label.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/state/state_machine.hpp"
#include "ui/qt/state/visualization.hpp"
#include <app/core/action/base_action.hpp>
#include <app/component/object3d/label.hpp>
#include <unordered_set>
#include <vector>

namespace VTX::UI::QT::Action::Label
{
	void Orient::execute()
	{
	 App::Component::Object3D::Helper::AABB aabb = App::Component::Object3D::Helper::AABB();

		for ( App::Component::Object3D::Label * const label : _labels )
			aabb.extend( label->getAABB() );

		QT_APP()
			->getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->orientCameraController( aabb );
	}
} // namespace VTX::UI::QT::Action::Label
