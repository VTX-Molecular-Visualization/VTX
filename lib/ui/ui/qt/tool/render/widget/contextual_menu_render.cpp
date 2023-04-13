#include "contextual_menu_render.hpp"
#include "qt/action/main.hpp"
#include "qt/application_qt.hpp"
#include "qt/dialog.hpp"
#include "qt/state/state_machine.hpp"
#include "qt/state/visualization.hpp"
#include "qt/tool/session/dialog.hpp"
#include <app/old_app/action/action_manager.hpp>
#include <app/old_app/action/main.hpp>
#include <app/old_app/action/scene.hpp>
#include <app/old_app/action/setting.hpp>

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

		//_measurementModeMenu = new QMenu( this );
		//_measurementModeMenu->setTitle( "Measurement" );
		//_addMeasurementModeActionInMenu( int( Controller::MeasurementPicker::Mode::DISTANCE ), "Distance" );
		////_addMeasurementModeActionInMenu( int( Controller::MeasurementPicker::Mode::DISTANCE_TO_CYCLE ),
		////								 "Distance to cycle" );
		//_addMeasurementModeActionInMenu( int( Controller::MeasurementPicker::Mode::ANGLE ), "Angle" );
		//_addMeasurementModeActionInMenu( int( Controller::MeasurementPicker::Mode::DIHEDRAL_ANGLE ), "Dihedral angle"
		//); _measurementModeAction = addMenu( _measurementModeMenu ); _measurementModeAction->setCheckable( true );

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

		// addSection( "Render" );
		//_projectionMenu = new QMenu( "Projection", this );
		// Util::UI::fillMenu( *_projectionMenu,
		//					int( Settings::VTXSettings::CameraProjection::COUNT ),
		//					Settings::VTXSettings::CAMERA_PROJECTION_STR,
		//					true );
		// addMenu( _projectionMenu );

		//_backgroundColorMenu = new QMenu( "Background color", this );
		// Util::UI::fillMenu(
		//	*_backgroundColorMenu, int( Renderer::DEFAULT_BACKGROUND::COUNT ), Renderer::DEFAULT_BACKGROUND_STR );
		// addMenu( _backgroundColorMenu );

		//_renderSettingPreset = new QMenu( "Apply render settings", this );
		// for ( int i = 0; i < Model::Renderer::RenderEffectPresetLibrary::get().getPresetCount(); i++ )
		//{
		//	const Model::Renderer::RenderEffectPreset & preset
		//		= *Model::Renderer::RenderEffectPresetLibrary::get().getPreset( i );

		//	QAction * const action = new QAction( QString::fromStdString( preset.getName() ), _renderSettingPreset );
		//	action->setData( i );
		//	action->setCheckable( true );
		//	_renderSettingPreset->addAction( action );
		//}
		// QAction * const openRenderEffectSettingsAction = new QAction( "Settings...", _renderSettingPreset );
		// openRenderEffectSettingsAction->setData( -1 );
		//_renderSettingPreset->addAction( openRenderEffectSettingsAction );
		// addMenu( _renderSettingPreset );

		addSection( "Action" );
		addAction( "Reset Camera", this, &ContextualMenuRender::_resetCameraAction );
		addAction( "Show All", this, &ContextualMenuRender::_showAllMoleculesAction );

		// addSection( "Overlays" );
		//_overlaysMenu = new QMenu( "Overlays", this );

		// QAction * const actionShowAllOverlays = new QAction( "Show all", _overlaysMenu );
		// actionShowAllOverlays->setData( SHOW_ALL_OVERLAYS_DATA_VALUE );
		//_overlaysMenu->addAction( actionShowAllOverlays );

		// QAction * const actionHideAllOverlays = new QAction( "Hide all", _overlaysMenu );
		// actionHideAllOverlays->setData( HIDE_ALL_OVERLAYS_DATA_VALUE );
		//_overlaysMenu->addAction( actionHideAllOverlays );
		//_overlaysMenu->addSeparator();
		// Util::UI::fillMenu( *_overlaysMenu,
		//					int( Widget::Render::Overlay::OVERLAY::COUNT ),
		//					Widget::Render::Overlay::OVERLAY_NAMES_STR,
		//					true );
		// addMenu( _overlaysMenu );

		// addSection( "Image" );
		// addAction( "Snapshot", this, &ContextualMenuRender::_takeSnapshotAction );
		// addAction( "Export Image...", this, &ContextualMenuRender::_exportImageAction );

		_refreshPickerMode();
		//_refreshSelectionGranularityMenu();
		//_refreshCameraProjection();
		//_refreshAppliedRenderSettingPreset();
		//_refreshOverlayVisibilityMenu();
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

	void ContextualMenuRender::_loadMoleculeAction() const { QT::Tool::Session::Dialog::openLoadMoleculeDialog(); }
	void ContextualMenuRender::_downloadMoleculeAction() const
	{
		QT::Tool::Session::Dialog::openDownloadMoleculeDialog();
	}
	void ContextualMenuRender::_showAllMoleculesAction() const
	{
		VTX_ACTION( new VTX::Action::Scene::ShowAllMolecules() );
	}
	void ContextualMenuRender::_resetCameraAction() const
	{
		VTX_ACTION( new QT::Action::Main::ResetCameraController() );
	}

	void ContextualMenuRender::_setPickerToSelection() const
	{
		VTX_ACTION( new VTX::Action::Main::ChangePicker( ID::Controller::PICKER ) );
	}
	// void ContextualMenuRender::_setPickerToMeasurement() const
	//{
	//	VTX_ACTION( new VTX::Action::Main::ChangePicker( ID::Controller::MEASUREMENT ) );
	// }

	void ContextualMenuRender::_setSelectionGranularityAction( QAction * p_action ) const
	{
		const VTX::Selection::Granularity granularity
			= VTX::Selection::Granularity( p_action->property( SELECTION_GRANULARITY_PROPERTY_NAME ).toInt() );
		VTX_ACTION( new VTX::Action::Setting::ChangeSelectionGranularity( granularity ) );
	}

	// void ContextualMenuRender::_setMeasurementMode( QAction * p_action ) const
	//{
	//	const Controller::MeasurementPicker::Mode mode
	//		= Controller::MeasurementPicker::Mode( p_action->property( MEASUREMENT_MODE_PROPERTY_NAME ).toInt() );
	//	VTX_ACTION( new Action::Main::ChangePicker( ID::Controller::MEASUREMENT, int( mode ) ) );
	// }

} // namespace VTX::UI::QT::Tool::Render::Widget
