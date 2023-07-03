#include "ui/old_ui/ui/widget/main_menu/camera/viewpoint_block.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/qt/action/viewpoint.hpp"
#include <app/old/action/viewpoint.hpp>
#include <app/old/event/global.hpp>
#include <app/old/component/video/path.hpp>
#include <app/old/application/selection/selection.hpp>
#include <app/old/mvc.hpp>
#include <app/old/component/render/camera.hpp>
#include <app/old/application/selection/selection_manager.hpp>
#include <app/old/vtx_app.hpp>

namespace VTX::UI::Widget::MainMenu::Camera
{
	ViewpointBlock::ViewpointBlock( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
	{
		_registerEvent( VTX::App::Old::Event::Global::SELECTION_CHANGE );
	};

	void ViewpointBlock::receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == App::Old::Event::Global::SELECTION_CHANGE )
		{
			const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Application::Selection::SelectionModel *> & castedEvent
				= dynamic_cast<const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Application::Selection::SelectionModel *> &>( p_event );

			const App::Old::Application::Selection::SelectionModel * const selectionModel = castedEvent.get();
			_enableDeleteButtonState( selectionModel->hasItemOfType( App::Old::ID::Model::MODEL_VIEWPOINT ) );
		}
	}

	void ViewpointBlock::_setupUi( const QString & p_name )
	{
		MenuToolBlockWidget::_setupUi( p_name );

		_createViewpointButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "createViewpointButton" );
		_createViewpointButton->setData( "Create", ":/sprite/add_viewpoint.png", Qt::Orientation::Horizontal );
		pushButton( *_createViewpointButton, 0 );

		_deleteViewpointButton
			= WidgetFactory::get().instantiateWidget<MenuToolButtonWidget>( this, "deleteViewpointButton" );
		_deleteViewpointButton->setData( "Delete", ":/sprite/delete_viewpoint.png", Qt::Orientation::Horizontal );
		pushButton( *_deleteViewpointButton, 0 );

		_enableDeleteButtonState( false );

		validate();
	}
	void ViewpointBlock::_setupSlots()
	{
		_createViewpointButton->setTriggerAction( this, &ViewpointBlock::_createViewpointAction );
		_deleteViewpointButton->setTriggerAction( this, &ViewpointBlock::_deleteViewpointAction );
	}

	void ViewpointBlock::localize() { setTitle( "Viewpoints" ); }

	void ViewpointBlock::_createViewpointAction() const { VTX_ACTION( new QT::Action::Viewpoint::Create() ); }
	void ViewpointBlock::_deleteViewpointAction() const
	{
		std::vector<App::Old::Component::Object3D::Viewpoint *> viewpointsInSelection = std::vector<App::Old::Component::Object3D::Viewpoint *>();
		const App::Old::Application::Selection::SelectionModel &		selection = VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel();

		viewpointsInSelection.reserve( selection.getItems().size() );

		for ( const App::Old::Core::Model::ID & modelID : selection.getItems() )
		{
			const App::Old::VTX_ID & modelTypeID = VTX::MVC_MANAGER().getModelTypeID( modelID );
			if ( modelTypeID == App::Old::ID::Model::MODEL_VIEWPOINT )
			{
				App::Old::Component::Object3D::Viewpoint & viewpoint = VTX::MVC_MANAGER().getModel<App::Old::Component::Object3D::Viewpoint>( modelID );
				viewpointsInSelection.emplace_back( &viewpoint );
			}
		}

		VTX_ACTION( new QT::Action::Viewpoint::Delete( viewpointsInSelection ) );
	}

	void ViewpointBlock::_enableDeleteButtonState( const bool p_enable )
	{
		_deleteViewpointButton->setEnabled( p_enable );
	}

} // namespace VTX::UI::Widget::MainMenu::Camera
