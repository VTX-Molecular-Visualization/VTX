#include "visualization_quick_access.hpp"
#include "action/action_manager.hpp"
#include "action/selection.hpp"
#include "model/selection.hpp"
#include "selection/selection_manager.hpp"
#include "setting.hpp"
#include <QHBoxLayout>
#include <QIcon>
#include <QMenu>
#include <set>

namespace VTX::UI::Widget::Render::Overlay
{
	VisualizationQuickAccess::VisualizationQuickAccess( QWidget * p_parent ) : BaseOverlay( p_parent )
	{
		_registerEvent( Event::Global::CONTROLLER_CHANGE );
		_registerEvent( Event::Global::SETTINGS_CHANGE );
	};

	void VisualizationQuickAccess::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::CONTROLLER_CHANGE )
			_refreshController();

		if ( p_event.name == Event::Global::SETTINGS_CHANGE )
		{
			const Event::VTXEventRef<std::set<Setting::PARAMETER>> & castedEvent
				= dynamic_cast<const Event::VTXEventRef<std::set<Setting::PARAMETER>> &>( p_event );

			if ( castedEvent.ref.find( Setting::PARAMETER::SELECTION_GRANULARITY ) != castedEvent.ref.cend() )
				_refreshSelectionGranularity();
		}
	}

	void VisualizationQuickAccess::_setupUi( const QString & p_name )
	{
		BaseOverlay::_setupUi( p_name );

		QAction * orientAction = addAction( QIcon( ":/sprite/camera_orient_icon.png" ), "" );
		_orientWidget		   = dynamic_cast<QToolButton *>( widgetForAction( orientAction ) );

		QMenu * changeControllerMenu = new QMenu( this );
		for ( int i = 0; i < CONTROLLERS.size(); i++ )
		{
			const MenuItemDataRef<ID::VTX_ID> & data = CONTROLLERS[ i ];
			QAction * action = changeControllerMenu->addAction( QIcon( data.iconPath ), data.name );
			action->setProperty( CONTROLLER_PROPERTY_NAME, QVariant( i ) );
		}
		_changeControllerWidget = dynamic_cast<QToolButton *>( widgetForAction( addMenu( changeControllerMenu ) ) );

		QMenu * const changeSelectionGranularityMenu = new QMenu( this );
		for ( int i = 0; i < SELECTION_GRANULARITY.size(); i++ )
		{
			const MenuItemData<VTX::Selection::SelectionGranularity> & data = SELECTION_GRANULARITY[ i ];
			QAction * action = changeSelectionGranularityMenu->addAction( QIcon( data.iconPath ), data.name );
			action->setProperty( GRANULARITY_PROPERTY_NAME, QVariant( int( data.data ) ) );
		}
		_changeSelectionGranularityWidget
			= dynamic_cast<QToolButton *>( widgetForAction( addMenu( changeSelectionGranularityMenu ) ) );

		_refreshController();
		_refreshSelectionGranularity();
		_refreshSize();
	}

	void VisualizationQuickAccess::_setupSlots()
	{
		BaseOverlay::_setupSlots();

		connect( _orientWidget, &QToolButton::clicked, this, &VisualizationQuickAccess::_orientAction );

		connect( _changeControllerWidget->menu(),
				 QOverload<QAction *>::of( &QMenu::triggered ),
				 this,
				 &VisualizationQuickAccess::_changeControllerAction );

		connect( _changeSelectionGranularityWidget->menu(),
				 QOverload<QAction *>::of( &QMenu::triggered ),
				 this,
				 &VisualizationQuickAccess::_changeSelectionGranularityAction );
	}

	void VisualizationQuickAccess::localize() {};

	void VisualizationQuickAccess::_refreshController()
	{
		const ID::VTX_ID & currentControllerID = VTXApp::get()
													 .getStateMachine()
													 .getState<State::Visualization>( ID::State::VISUALIZATION )
													 ->getCurrentCameraControllerID();

		for ( const MenuItemDataRef<ID::VTX_ID> & data : CONTROLLERS )
		{
			if ( data.data == currentControllerID )
			{
				_changeControllerWidget->setIcon( QIcon( data.iconPath ) );
				break;
			}
		}
	}
	void VisualizationQuickAccess::_refreshSelectionGranularity()
	{
		const VTX::Selection::SelectionGranularity granularity = VTX_SETTING().getSelectionGranularity();

		for ( const MenuItemData<VTX::Selection::SelectionGranularity> & data : SELECTION_GRANULARITY )
		{
			if ( data.data == granularity )
			{
				_changeSelectionGranularityWidget->setIcon( QIcon( data.iconPath ) );
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
		const int controllerID = p_action->property( CONTROLLER_PROPERTY_NAME ).toInt();
		VTXApp::get()
			.getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->setCameraController( CONTROLLERS[ controllerID ].data );
	}

	void VisualizationQuickAccess::_changeSelectionGranularityAction( const QAction * const p_action )
	{
		const VTX::Selection::SelectionGranularity granularity
			= VTX::Selection::SelectionGranularity( p_action->property( GRANULARITY_PROPERTY_NAME ).toInt() );

		VTX_SETTING().setSelectionGranularity( granularity );
	}

} // namespace VTX::UI::Widget::Render::Overlay
