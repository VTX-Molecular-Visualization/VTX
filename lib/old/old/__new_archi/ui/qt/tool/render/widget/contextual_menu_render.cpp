#include "contextual_menu_render.hpp"
#include "__new_archi/ui/qt/application_qt.hpp"
#include "__new_archi/ui/qt/state/visualization.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "action/scene.hpp"
#include "action/setting.hpp"
#include "ui/dialog.hpp"

namespace VTX::UI::QT::Tool::Render::Widget
{
	ContextualMenuRender::ContextualMenuRender( QWidget * const p_parent ) :
		QT::Widget::ContextualMenu::BaseContextualMenu( p_parent )
	{
		_registerEvent( Event::Global::PICKER_MODE_CHANGE );
	}
	ContextualMenuRender ::~ContextualMenuRender() {}

	void ContextualMenuRender ::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::PICKER_MODE_CHANGE )
		{
			_refreshPickerMode();
		}
	}

	void ContextualMenuRender::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }
	void ContextualMenuRender::_setupSlots()
	{
		addSection( "Loading" );
		addAction( "Load Molecule", this, &ContextualMenuRender::_loadMoleculeAction );
		addAction( "Download Molecule", this, &ContextualMenuRender::_downloadMoleculeAction );

		addSection( "Mouse Mode" );
		_selectionModeAction = addAction( "Selection", this, &ContextualMenuRender::_setPickerToSelection );
		_selectionModeAction->setCheckable( true );
		//_measurementModeAction = addAction( "Measurement", this, &ContextualMenuRender::_setPickerToMeasurement );
		//_measurementModeAction->setCheckable( true );

		addSection( "Actions" );
		addAction( "Show All", this, &ContextualMenuRender::_showAllMoleculesAction );

		addSection( "Selection" );
		QMenu * const selectionGranularityMenu = new QMenu( this );
		selectionGranularityMenu->setTitle( "Selection target" );
		selectionGranularityMenu->addAction( "Atom" )->setProperty( SELECTION_GRANULARITY_PROPERTY_NAME,
																	int( VTX::Selection::Granularity::ATOM ) );
		selectionGranularityMenu->addAction( "Residue" )
			->setProperty( SELECTION_GRANULARITY_PROPERTY_NAME, int( VTX::Selection::Granularity::RESIDUE ) );
		selectionGranularityMenu->addAction( "Chain" )->setProperty( SELECTION_GRANULARITY_PROPERTY_NAME,
																	 int( VTX::Selection::Granularity::CHAIN ) );
		selectionGranularityMenu->addAction( "Molecule" )
			->setProperty( SELECTION_GRANULARITY_PROPERTY_NAME, int( VTX::Selection::Granularity::MOLECULE ) );
		connect(
			selectionGranularityMenu, &QMenu::triggered, this, &ContextualMenuRender::_setSelectionGranularityAction );

		addMenu( selectionGranularityMenu );

		// addSection( "Measurement" );
		// QMenu * const measurementModeMenu = new QMenu( this );
		// measurementModeMenu->setTitle( "Measurement" );
		// measurementModeMenu->addAction( "Distance" )
		//	->setProperty( MEASUREMENT_MODE_PROPERTY_NAME, int( Controller::MeasurementPicker::Mode::DISTANCE ) );
		//// measurementModeMenu->addAction( "Distance to cycle" )
		////	->setProperty( MEASUREMENT_MODE_PROPERTY_NAME,
		////				   int( Controller::MeasurementPicker::Mode::DISTANCE_TO_CYCLE ) );
		// measurementModeMenu->addAction( "Angle" )->setProperty( MEASUREMENT_MODE_PROPERTY_NAME,
		//														int( Controller::MeasurementPicker::Mode::ANGLE ) );
		// measurementModeMenu->addAction( "Dihedral angle" )
		//	->setProperty( MEASUREMENT_MODE_PROPERTY_NAME, int( Controller::MeasurementPicker::Mode::DIHEDRAL_ANGLE ) );
		// connect( measurementModeMenu, &QMenu::triggered, this, &ContextualMenuRender::_setMeasurementMode );

		// addMenu( measurementModeMenu );

		_refreshPickerMode();
	}

	void ContextualMenuRender::localize() {}

	void ContextualMenuRender::_refreshPickerMode() const
	{
		const ID::VTX_ID & pickerID = QT_APP()
										  ->getStateMachine()
										  .getState<QT::State::Visualization>( ID::State::VISUALIZATION )
										  ->getCurrentPickerID();

		_selectionModeAction->setChecked( pickerID == ID::Controller::PICKER );
		//_measurementModeAction->setChecked( pickerID == ID::Controller::MEASUREMENT );
	}

	void ContextualMenuRender::_loadMoleculeAction() const { UI::Dialog::openLoadMoleculeDialog(); }
	void ContextualMenuRender::_downloadMoleculeAction() const { UI::Dialog::openDownloadMoleculeDialog(); }
	void ContextualMenuRender::_showAllMoleculesAction() const { VTX_ACTION( new Action::Scene::ShowAllMolecules() ); }

	void ContextualMenuRender::_setPickerToSelection() const
	{
		VTX_ACTION( new Action::Main::ChangePicker( ID::Controller::PICKER ) );
	}
	// void ContextualMenuRender::_setPickerToMeasurement() const
	//{
	//	VTX_ACTION( new Action::Main::ChangePicker( ID::Controller::MEASUREMENT ) );
	// }

	void ContextualMenuRender::_setSelectionGranularityAction( QAction * p_action ) const
	{
		const VTX::Selection::Granularity granularity
			= VTX::Selection::Granularity( p_action->property( SELECTION_GRANULARITY_PROPERTY_NAME ).toInt() );
		VTX_ACTION( new Action::Setting::ChangeSelectionGranularity( granularity ) );
	}

	// void ContextualMenuRender::_setMeasurementMode( QAction * p_action ) const
	//{
	//	const Controller::MeasurementPicker::Mode mode
	//		= Controller::MeasurementPicker::Mode( p_action->property( MEASUREMENT_MODE_PROPERTY_NAME ).toInt() );
	//	VTX_ACTION( new Action::Main::ChangePicker( ID::Controller::MEASUREMENT, int( mode ) ) );
	// }

} // namespace VTX::UI::QT::Tool::Render::Widget
