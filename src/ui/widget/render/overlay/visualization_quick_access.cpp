#include "visualization_quick_access.hpp"
#include "action/action_manager.hpp"
#include "action/selection.hpp"
#include "model/selection.hpp"
#include "selection/selection_manager.hpp"
#include <QHBoxLayout>
#include <QIcon>
#include <QMenu>

namespace VTX::UI::Widget::Render::Overlay
{
	VisualizationQuickAccess::VisualizationQuickAccess( QWidget * p_parent ) : BaseOverlay( p_parent )
	{
		_registerEvent( Event::Global::CONTROLLER_CHANGE );
	};

	void VisualizationQuickAccess::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::CONTROLLER_CHANGE )
			_refreshController();
	}

	void VisualizationQuickAccess::_setupUi( const QString & p_name )
	{
		BaseOverlay::_setupUi( p_name );

		QAction * orientAction = addAction( QIcon( ":/sprite/camera_orient_icon.png" ), "" );
		_orientWidget		   = dynamic_cast<QToolButton *>(widgetForAction( orientAction ));

		QMenu * changeControllerMenu = new QMenu( this );
		for ( int i = 0; i < CONTROLLERS.size(); i++ )
		{
			const MenuItemData<ID::VTX_ID> & data = CONTROLLERS[ i ];
			QAction *			   action = changeControllerMenu->addAction( QIcon( data.iconPath ), data.name );
			action->setProperty( CONTROLLER_PROPERTY_NAME, QVariant( i ) );
		}
		_changeControllerWidget = dynamic_cast<QToolButton *>( widgetForAction( addMenu( changeControllerMenu ) ) );

		_refreshController();
	}

	void VisualizationQuickAccess::_setupSlots()
	{
		BaseOverlay::_setupSlots();
		
		connect( _orientWidget, &QToolButton::clicked, this, &VisualizationQuickAccess::_orientAction );

		connect( _changeControllerWidget->menu(),
				 QOverload<QAction *>::of( &QMenu::triggered ),
				 this,
				 &VisualizationQuickAccess::_changeControllerAction );
		
	}

	void VisualizationQuickAccess::localize() {};

	void VisualizationQuickAccess::_refreshController()
	{
		const ID::VTX_ID currentControllerID = VTXApp::get()
												   .getStateMachine()
												   .getState<State::Visualization>( ID::State::VISUALIZATION )
												   ->getCurrentCameraControllerID();

		for ( const MenuItemData<ID::VTX_ID> & data : CONTROLLERS )
		{
			if ( data.data == currentControllerID )
			{
				_changeControllerWidget->setIcon( QIcon( data.iconPath ) );
				break;
			}
		}
	}

	void VisualizationQuickAccess::_orientAction()
	{
		const Model::Selection & selection = VTX::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new Action::Selection::Orient( selection ) );
	}
	void VisualizationQuickAccess::_changeControllerAction( const QAction * const p_action )
	{
		const int p_controllerID = p_action->property( CONTROLLER_PROPERTY_NAME ).toInt();
		VTXApp::get()
			.getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->setCameraController( CONTROLLERS[ p_controllerID ].data );
	}

} // namespace VTX::UI::Widget::Render::Overlay
