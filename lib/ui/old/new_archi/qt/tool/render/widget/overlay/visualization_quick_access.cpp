#include "ui/qt/tool/render/widget/overlay/visualization_quick_access.hpp"
#include "ui/old_ui/action/main.hpp"
#include "ui/qt/action/selection.hpp"
#include "ui/qt/application_qt.hpp"
#include "ui/qt/state/state_machine.hpp"
#include "ui/qt/state/visualization.hpp"
#include <QHBoxLayout>
#include <QIcon>
#include <QMenu>
#include <app/old/action/main.hpp>
#include <app/old/action/selection.hpp>
#include <app/old/action/setting.hpp>
#include <app/old/application/selection/selection.hpp>
#include <app/old/application/selection/selection_manager.hpp>
#include <app/old/application/setting.hpp>
#include <app/old/event/global.hpp>
#include <set>

namespace VTX::UI::QT::Tool::Render::Widget::Overlay
{
	VisualizationQuickAccess::VisualizationQuickAccess( QWidget * p_parent ) : BaseOverlay( p_parent )
	{
		_registerEvent( VTX::App::Old::Event::Global::CONTROLLER_CHANGE );
		_registerEvent( VTX::App::Old::Event::Global::PICKER_MODE_CHANGE );
		_registerEvent( VTX::App::Old::Event::Global::SETTINGS_CHANGE );
	};

	void VisualizationQuickAccess::receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Old::Event::Global::CONTROLLER_CHANGE )
		{
			_refreshController();
		}
		else if ( p_event.name == VTX::App::Old::Event::Global::PICKER_MODE_CHANGE )
		{
			_refreshPicker();
		}
		else if ( p_event.name == VTX::App::Old::Event::Global::SETTINGS_CHANGE )
		{
			const VTX::App::Old::Core::Event::VTXEventArg<const std::set<VTX::App::Old::Application::Setting::PARAMETER> &> &
				castedEvent
				= dynamic_cast<const VTX::App::Old::Core::Event::VTXEventArg<
					const std::set<VTX::App::Old::Application::Setting::PARAMETER> &> &>( p_event );

			if ( castedEvent.get().find( VTX::App::Old::Application::Setting::PARAMETER::SELECTION_GRANULARITY )
				 != castedEvent.get().cend() )
				_refreshSelectionGranularity();
		}
	}

	void VisualizationQuickAccess::_setupUi( const QString & p_name )
	{
		BaseOverlay::_setupUi( p_name );

		QAction * const orientAction = addAction( QIcon( ":/sprite/camera_orient_icon.png" ), "" );
		_orientWidget				 = dynamic_cast<QToolButton *>( widgetForAction( orientAction ) );

		QMenu * const changeCameraControllerMenu = new QMenu( this );
		for ( int i = 0; i < CAMERA_CONTROLLERS.size(); i++ )
		{
			const MenuItemDataRef<App::Old::VTX_ID> & data = CAMERA_CONTROLLERS[ i ];
			QAction * const action = changeCameraControllerMenu->addAction( QIcon( data.iconPath ), data.name );
			action->setProperty( CAMERA_CONTROLLER_PROPERTY_NAME, QVariant( i ) );
		}
		_changeCameraControllerWidget
			= dynamic_cast<QToolButton *>( widgetForAction( addMenu( changeCameraControllerMenu ) ) );

		QMenu * const changePickerControllerMenu = new QMenu( this );
		for ( int i = 0; i < PICKER_CONTROLLERS.size(); i++ )
		{
			const MenuItemDataRef<App::Old::VTX_ID> & data = PICKER_CONTROLLERS[ i ];
			QAction * const action = changePickerControllerMenu->addAction( QIcon( data.iconPath ), data.name );
			action->setProperty( PICKER_CONTROLLER_PROPERTY_NAME, QVariant( i ) );
		}
		_changePickerControllerWidget
			= dynamic_cast<QToolButton *>( widgetForAction( addMenu( changePickerControllerMenu ) ) );

		QMenu * const changeSelectionGranularityMenu = new QMenu( this );
		for ( int i = 0; i < SELECTION_GRANULARITY.size(); i++ )
		{
			const MenuItemData<VTX::App::Old::Application::Selection::GRANULARITY> & data = SELECTION_GRANULARITY[ i ];
			QAction * const action = changeSelectionGranularityMenu->addAction( QIcon( data.iconPath ), data.name );
			action->setProperty( GRANULARITY_PROPERTY_NAME, QVariant( int( data.data ) ) );
		}
		_changeSelectionGranularityQAction = addMenu( changeSelectionGranularityMenu );
		_changeSelectionGranularityWidget
			= dynamic_cast<QToolButton *>( widgetForAction( _changeSelectionGranularityQAction ) );

		QMenu * const changeMeasurementModeMenu = new QMenu( this );
		for ( int i = 0; i < MEASUREMENT_MODE.size(); i++ )
		{
			const MenuItemData<Controller::MeasurementPicker::Mode> & data = MEASUREMENT_MODE[ i ];
			QAction * const action = changeMeasurementModeMenu->addAction( QIcon( data.iconPath ), data.name );
			action->setProperty( MEASUREMENT_MODE_PROPERTY_NAME, QVariant( int( data.data ) ) );
		}
		_changeMeasurementModeQAction = addMenu( changeMeasurementModeMenu );
		_changeMeasurementModeWidget  = dynamic_cast<QToolButton *>( widgetForAction( _changeMeasurementModeQAction ) );

		_refreshController();
		_refreshPicker();
		_refreshSize();
	}

	void VisualizationQuickAccess::_setupSlots()
	{
		BaseOverlay::_setupSlots();

		connect( _orientWidget, &QToolButton::clicked, this, &VisualizationQuickAccess::_orientAction );

		connect( _changeCameraControllerWidget->menu(),
				 QOverload<QAction *>::of( &QMenu::triggered ),
				 this,
				 &VisualizationQuickAccess::_changeCameraControllerAction );

		connect( _changePickerControllerWidget->menu(),
				 QOverload<QAction *>::of( &QMenu::triggered ),
				 this,
				 &VisualizationQuickAccess::_changePickerControllerAction );

		connect( _changeSelectionGranularityWidget->menu(),
				 QOverload<QAction *>::of( &QMenu::triggered ),
				 this,
				 &VisualizationQuickAccess::_changeSelectionGranularityAction );

		connect( _changeMeasurementModeWidget->menu(),
				 QOverload<QAction *>::of( &QMenu::triggered ),
				 this,
				 &VisualizationQuickAccess::_changeMeasurementModeAction );
	}

	void VisualizationQuickAccess::localize() {};

	void VisualizationQuickAccess::_refreshController()
	{
		const QT::State::Visualization * const visualizationState
			= QT_APP()->getStateMachine().getState<QT::State::Visualization>( ID::State::VISUALIZATION );

		const App::Old::VTX_ID & currentCameraControllerID = visualizationState->getCurrentCameraControllerID();
		for ( const MenuItemDataRef<App::Old::VTX_ID> & data : CAMERA_CONTROLLERS )
		{
			if ( data.data == currentCameraControllerID )
			{
				_changeCameraControllerWidget->setIcon( QIcon( data.iconPath ) );
				break;
			}
		}
	}
	void VisualizationQuickAccess::_refreshPicker()
	{
		const QT::State::Visualization * const visualizationState
			= QT_APP()->getStateMachine().getState<QT::State::Visualization>( ID::State::VISUALIZATION );
		const App::Old::VTX_ID & currentPickerID = visualizationState->getCurrentPickerID();
		for ( const MenuItemDataRef<App::Old::VTX_ID> & data : PICKER_CONTROLLERS )
		{
			if ( data.data == currentPickerID )
			{
				_changePickerControllerWidget->setIcon( QIcon( data.iconPath ) );
				break;
			}
		}

		_changeSelectionGranularityQAction->setVisible( currentPickerID == ID::Controller::PICKER );
		_changeMeasurementModeQAction->setVisible( currentPickerID == ID::Controller::MEASUREMENT );

		if ( currentPickerID == ID::Controller::PICKER )
			_refreshSelectionGranularity();
		else if ( currentPickerID == ID::Controller::MEASUREMENT )
			_refreshMeasurementMode();
	}
	void VisualizationQuickAccess::_refreshSelectionGranularity()
	{
		const VTX::App::Old::Application::Selection::GRANULARITY granularity = VTX_SETTING().getSelectionGranularity();

		for ( const MenuItemData<VTX::App::Old::Application::Selection::GRANULARITY> & data : SELECTION_GRANULARITY )
		{
			if ( data.data == granularity )
			{
				_changeSelectionGranularityWidget->setIcon( QIcon( data.iconPath ) );
				break;
			}
		}
	}
	void VisualizationQuickAccess::_refreshMeasurementMode()
	{
		const Controller::MeasurementPicker::Mode currentMeasurementMode
			= QT_APP()
				  ->getStateMachine()
				  .getState<State::Visualization>( ID::State::VISUALIZATION )
				  ->getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT )
				  ->getCurrentMode();

		for ( const MenuItemData<Controller::MeasurementPicker::Mode> & data : MEASUREMENT_MODE )
		{
			if ( data.data == currentMeasurementMode )
			{
				_changeMeasurementModeWidget->setIcon( QIcon( data.iconPath ) );
				break;
			}
		}
	}

	void VisualizationQuickAccess::_orientAction()
	{
		const App::Old::Application::Selection::SelectionModel & selection
			= VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel();
		VTX_ACTION( new QT::Action::Selection::Orient( selection ) );
	}
	void VisualizationQuickAccess::_changeCameraControllerAction( const QAction * const p_action )
	{
		const int controllerID = p_action->property( CAMERA_CONTROLLER_PROPERTY_NAME ).toInt();
		QT_APP()
			->getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->setCameraController( CAMERA_CONTROLLERS[ controllerID ].data );
	}

	void VisualizationQuickAccess::_changePickerControllerAction( const QAction * const p_action )
	{
		const int controllerID = p_action->property( PICKER_CONTROLLER_PROPERTY_NAME ).toInt();
		VTX_ACTION( new VTX::Action::Main::ChangePicker( PICKER_CONTROLLERS[ controllerID ].data ) );
	}

	void VisualizationQuickAccess::_changeSelectionGranularityAction( const QAction * const p_action )
	{
		const VTX::App::Old::Application::Selection::GRANULARITY granularity
			= VTX::App::Old::Application::Selection::GRANULARITY( p_action->property( GRANULARITY_PROPERTY_NAME ).toInt() );

		VTX_ACTION( new VTX::App::Old::Action::Setting::ChangeSelectionGranularity( granularity ) );
	}
	void VisualizationQuickAccess::_changeMeasurementModeAction( const QAction * const p_action )
	{
		const Controller::MeasurementPicker::Mode measurementMode
			= Controller::MeasurementPicker::Mode( p_action->property( MEASUREMENT_MODE_PROPERTY_NAME ).toInt() );

		VTX_ACTION( new VTX::Action::Main::ChangePicker( ID::Controller::MEASUREMENT, int( measurementMode ) ) );
	}

} // namespace VTX::UI::QT::Tool::Render::Widget::Overlay