#include "app/controller/picker/selection.hpp"
#include "app/core/input/input_manager.hpp"
#include <app/application/scene_utility.hpp>
#include <app/application/selection/selection.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/component/scene/pickable.hpp>
#include <app/core/action/action_system.hpp>
#include <app/core/ecs/registry.hpp>
#include <app/core/renderer/renderer_system.hpp>
#include <optional>
#include <util/logger.hpp>

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

namespace VTX::App::Controller::Picker
{
	void Selection::setActive( const bool p_active )
	{
		// Connect/disconnect mouse events.
		if ( p_active )
		{
			_mouseLeftClickCallbackID = INPUT_MANAGER().onMouseLeftClicked +=
				[ this ]( const Vec2i & p_mousePosition ) { _onMouseLeftClick( p_mousePosition ); };

			_mouseLeftDoubleClickCallbackID = INPUT_MANAGER().onMouseLeftDoubleClicked +=
				[ this ]( const Vec2i & p_mousePosition ) { _onMouseLeftDoubleClick( p_mousePosition ); };

			_mouseRightClickCallbackID = INPUT_MANAGER().onMouseRightClicked +=
				[ this ]( const Vec2i & p_mousePosition ) { _onMouseRightClick( p_mousePosition ); };
		}
		else
		{
			INPUT_MANAGER().onMouseLeftClicked -= _mouseLeftClickCallbackID;
			INPUT_MANAGER().onMouseLeftDoubleClicked -= _mouseLeftDoubleClickCallbackID;
			INPUT_MANAGER().onMouseRightClicked -= _mouseRightClickCallbackID;
		}
	}

	void Selection::_onMouseLeftClick( const Vec2i & p_mousePos )
	{
		const PickingInfo pickingInfo
			= PickingInfo( App::RENDERER_SYSTEM().getPickedIds( p_mousePos.x, p_mousePos.y ) );

		VTX_INFO( "PickingInfo : {}, {}.", pickingInfo.getFirst(), pickingInfo.getSecond() );

		_performSelection( pickingInfo );
		_lastPickingInfo = pickingInfo;
	}

	void Selection::_onMouseRightClick( const Vec2i & p_mousePos )
	{
		const PickingInfo pickingInfo
			= PickingInfo( App::RENDERER_SYSTEM().getPickedIds( p_mousePos.x, p_mousePos.y ) );

		if ( !_isTargetSelected( pickingInfo ) )
		{
			_performSelection( pickingInfo );
			_lastPickingInfo = pickingInfo;
		}

		// const QT::Widget::Renderer::Panel * const renderWidget = QT_APP()->getMainWindow().getRender();
		// const QPoint position = renderWidget->mapToGlobal( QPoint( p_mousePos.x, p_mousePos.y ) );

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

	void Selection::_onMouseLeftDoubleClick( const Vec2i & p_mousePos )
	{
		const PickingInfo pickingInfo
			= PickingInfo( App::RENDERER_SYSTEM().getPickedIds( p_mousePos.x, p_mousePos.y ) );

		if ( !pickingInfo.hasValue() || pickingInfo != _lastPickingInfo )
			return;

		if ( INPUT_MANAGER().isModifierExclusive( Core::Input::ModifierEnum::None ) )
		{
			// App::VTX_ACTION().execute<QT::Action::Selection::Orient>( App::CURRENT_SELECTION() );
		}
	}

	void Selection::_performSelection( const PickingInfo & p_pickingInfo ) const
	{
		// Append to selection if CTRL modifier pressed.
		const App::Component::Scene::Pickable::PickType pickType
			= INPUT_MANAGER().isModifierExclusive( Core::Input::ModifierEnum::Ctrl )
				  ? App::Component::Scene::Pickable::PickType::TOGGLE
				  : App::Component::Scene::Pickable::PickType::SET;

		if ( !p_pickingInfo.hasValue() )
		{
			// Clear selection when the user clicks in void without modfiers.
			if ( pickType == App::Component::Scene::Pickable::PickType::SET )
			{
				App::CURRENT_SELECTION().clear();
			}
		}
		else
		{
			const auto pickableComponentOptional = _tryGetPickableFromUid( p_pickingInfo.getFirst() );

			if ( pickableComponentOptional )
				pickableComponentOptional->pick( p_pickingInfo, pickType );
		}
	}

	bool Selection::_isTargetSelected( const PickingInfo & p_pickingInfo ) const
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

} // namespace VTX::App::Controller::Picker
