#include "ui/old_ui/ui/widget/main_menu/camera/viewpoint_block.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/qt/action/viewpoint.hpp"
#include <app/action/viewpoint.hpp>
#include <app/event/global.hpp>
#include <app/component/video/path.hpp>
#include <app/application/selection/selection.hpp>
#include <app/mvc.hpp>
#include <app/component/render/camera.hpp>
#include <app/application/selection/selection_manager.hpp>
#include <app/vtx_app.hpp>

namespace VTX::UI::Widget::MainMenu::Camera
{
	ViewpointBlock::ViewpointBlock( QWidget * p_parent ) : MenuToolBlockWidget( p_parent )
	{
		_registerEvent( VTX::App::Event::Global::SELECTION_CHANGE );
	};

	void ViewpointBlock::receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == App::Event::Global::SELECTION_CHANGE )
		{
			const VTX::App::Core::Event::VTXEventArg<App::Application::Selection::SelectionModel *> & castedEvent
				= dynamic_cast<const VTX::App::Core::Event::VTXEventArg<App::Application::Selection::SelectionModel *> &>( p_event );

			const App::Application::Selection::SelectionModel * const selectionModel = castedEvent.get();
			_enableDeleteButtonState( selectionModel->hasItemOfType( App::ID::Model::MODEL_VIEWPOINT ) );
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
		std::vector<App::Component::Object3D::Viewpoint *> viewpointsInSelection = std::vector<App::Component::Object3D::Viewpoint *>();
		const App::Application::Selection::SelectionModel &		selection = VTX::App::Application::Selection::SelectionManager::get().getSelectionModel();

		viewpointsInSelection.reserve( selection.getItems().size() );

		for ( const App::Core::Model::ID & modelID : selection.getItems() )
		{
			const App::VTX_ID & modelTypeID = VTX::MVC_MANAGER().getModelTypeID( modelID );
			if ( modelTypeID == App::ID::Model::MODEL_VIEWPOINT )
			{
				App::Component::Object3D::Viewpoint & viewpoint = VTX::MVC_MANAGER().getModel<App::Component::Object3D::Viewpoint>( modelID );
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
