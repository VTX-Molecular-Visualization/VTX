#include "ui/qt/controller/selection_picker.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/input/input_manager.hpp"
#include "ui/qt/main_window.hpp"
#include "ui/qt/renderer_qt.hpp"
#include "ui/qt/tool/render/widget/render_widget.hpp"
#include <app/application/action/action_manager.hpp>
#include <app/application/scene_utility.hpp>
#include <app/application/selection/selection.hpp>
#include <app/component/scene/pickable.hpp>
#include <app/core/ecs/registry.hpp>
#include <app/vtx_app.hpp>
#include <optional>

namespace
{
	std::optional<const VTX::App::Component::Scene::Pickable> _tryGetPickableFromUid(
		const VTX::App::Core::UID::uid p_uid
	)
	{
		const VTX::App::Core::ECS::BaseEntity entity = VTX::App::Application::SceneUtility::findItemByUID( p_uid );

		if ( entity == VTX::App::Core::ECS::INVALID_ENTITY )
			return std::optional<VTX::App::Component::Scene::Pickable>();

		const VTX::App::Component::Scene::Pickable & pickableComponent
			= VTX::App::MAIN_REGISTRY().getComponent<VTX::App::Component::Scene::Pickable>( entity );

		return std::optional<const VTX::App::Component::Scene::Pickable>( pickableComponent );
	}
} // namespace

namespace VTX::UI::QT::Controller
{
	void SelectionPicker::init()
	{
		INPUT_MANAGER().onMouseLeftClicked.addCallback(
			this, [ this ]( Vec2i p_mousePosition ) { _onMouseLeftClick( p_mousePosition ); }
		);

		INPUT_MANAGER().onMouseLeftDoubleClicked.addCallback(
			this, [ this ]( Vec2i p_mousePosition ) { _onMouseLeftDoubleClick( p_mousePosition ); }
		);

		INPUT_MANAGER().onMouseRightClicked.addCallback(
			this, [ this ]( Vec2i p_mousePosition ) { _onMouseRightClick( p_mousePosition ); }
		);
	}

	void SelectionPicker::_onMouseLeftClick( const Vec2i & p_mousePos )
	{
		const PickingInfo pickingInfo = PickingInfo( QT_RENDERER().get().getPickedIds( p_mousePos.x, p_mousePos.y ) );

		_performSelection( pickingInfo );
		_lastPickingInfo = pickingInfo;
	}

	void SelectionPicker::_onMouseRightClick( const Vec2i & p_mousePos )
	{
		const PickingInfo pickingInfo = PickingInfo( QT_RENDERER().get().getPickedIds( p_mousePos.x, p_mousePos.y ) );

		if ( !_isTargetSelected( pickingInfo ) )
		{
			_performSelection( pickingInfo );
			_lastPickingInfo = pickingInfo;
		}

		const QT::Tool::Render::Widget::RenderWidget * const renderWidget = QT_APP()->getMainWindow().getRender();
		const QPoint position = renderWidget->mapToGlobal( QPoint( p_mousePos.x, p_mousePos.y ) );

		if ( App::CURRENT_SELECTION().isEmpty() )
		{
			//	QT_APP()->getMainWindow().getContextualMenu().pop( Tool::ContextualMenu::RENDER, position );
		}
		else
		{
			//	QT_APP()->getMainWindow().getContextualMenu().pop(
			//		Tool::ContextualMenu::SELECTION, &App::CURRENT_SELECTION(), position
			//);
		}
	}

	void SelectionPicker::_onMouseLeftDoubleClick( const Vec2i & p_mousePos )
	{
		const PickingInfo pickingInfo = PickingInfo( QT_RENDERER().get().getPickedIds( p_mousePos.x, p_mousePos.y ) );

		if ( !pickingInfo.hasValue() || pickingInfo != _lastPickingInfo )
			return;

		if ( INPUT_MANAGER().isModifierExclusive( Input::ModifierEnum::None ) )
		{
			// App::VTX_ACTION().execute<QT::Action::Selection::Orient>( App::CURRENT_SELECTION() );
		}
	}

	void SelectionPicker::_performSelection( const PickingInfo & p_pickingInfo ) const
	{
		// Append to selection if CTRL modifier pressed.
		const App::Application::Selection::AssignmentType assignmentType
			= INPUT_MANAGER().isModifierExclusive( Input::ModifierEnum::Ctrl )
				  ? App::Application::Selection::AssignmentType::APPEND
				  : App::Application::Selection::AssignmentType::SET;

		if ( !p_pickingInfo.hasValue() )
		{
			// Clear selection when the user clicks in void.
			if ( assignmentType == App::Application::Selection::AssignmentType::SET )
			{
				App::CURRENT_SELECTION().clear();
			}
		}
		else
		{
			const auto pickableComponentOptional = _tryGetPickableFromUid( p_pickingInfo.getFirst() );

			if ( pickableComponentOptional )
				pickableComponentOptional->pick( p_pickingInfo, assignmentType );
		}
	}

	bool SelectionPicker::_isTargetSelected( const PickingInfo & p_pickingInfo ) const
	{
		bool res = false;

		if ( p_pickingInfo.hasValue() )
		{
			const auto firstPickableComponentOptional = _tryGetPickableFromUid( p_pickingInfo.getFirst() );

			res = firstPickableComponentOptional
				  && firstPickableComponentOptional->isSelected( p_pickingInfo.getFirst() );

			if ( res && p_pickingInfo.hasTwoValues() )
			{
				const auto secondPickableComponentOptional = _tryGetPickableFromUid( p_pickingInfo.getSecond() );

				res = secondPickableComponentOptional
					  && secondPickableComponentOptional->isSelected( p_pickingInfo.getSecond() );
			}
		}

		return res;
	}

} // namespace VTX::UI::QT::Controller
