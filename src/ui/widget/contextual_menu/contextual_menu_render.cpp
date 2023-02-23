#include "contextual_menu_render.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "action/renderer.hpp"
#include "action/scene.hpp"
#include "action/setting.hpp"
#include "action/viewpoint.hpp"
#include "model/renderer/render_effect_preset_library.hpp"
#include "object3d/scene.hpp"
#include "setting.hpp"
#include "state/visualization.hpp"
#include "ui/dialog.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/renderer/default_background.hpp"
#include "ui/widget/settings/setting_widget_enum.hpp"
#include "util/filesystem.hpp"
#include "util/ui.hpp"
#include "vtx_app.hpp"
#include "worker/snapshoter.hpp"

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuRender::ContextualMenuRender( QWidget * const p_parent ) : BaseContextualMenu( p_parent )
	{
		_registerEvent( Event::Global::PICKER_MODE_CHANGE );
		_registerEvent( Event::Global::SETTINGS_CHANGE );
		_registerEvent( Event::Global::APPLIED_RENDER_EFFECT_CHANGE );
	}
	ContextualMenuRender ::~ContextualMenuRender() {}

	void ContextualMenuRender ::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::PICKER_MODE_CHANGE )
		{
			_refreshPickerMode();
		}
		else if ( p_event.name == Event::Global::APPLIED_RENDER_EFFECT_CHANGE )
		{
			_refreshAppliedRenderSettingPreset();
		}
		else if ( p_event.name == Event::Global::SETTINGS_CHANGE )
		{
			const Event::VTXEventRef<std::set<Setting::PARAMETER>> & castedEvent
				= dynamic_cast<const Event::VTXEventRef<std::set<Setting::PARAMETER>> &>( p_event );

			if ( castedEvent.ref.find( Setting::PARAMETER::CAMERA_PROJECTION ) != castedEvent.ref.end() )
			{
				_refreshCameraProjection();
			}
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
		_measurementModeAction = addAction( "Measurement", this, &ContextualMenuRender::_setPickerToMeasurement );
		_measurementModeAction->setCheckable( true );

		addSection( "Camera" );
		_projectionMenu = new QMenu( "Projection", this );
		Util::UI::fillMenu( *_projectionMenu,
							int( Settings::VTXSettings::CameraProjection::COUNT ),
							Settings::VTXSettings::CAMERA_PROJECTION_STR,
							true );
		addMenu( _projectionMenu );

		_backgroundColorMenu = new QMenu( "Background color", this );
		Util::UI::fillMenu(
			*_backgroundColorMenu, int( Renderer::DEFAULT_BACKGROUND::COUNT ), Renderer::DEFAULT_BACKGROUND_STR );
		addMenu( _backgroundColorMenu );

		_renderSettingPreset = new QMenu( "Apply render settings", this );
		for ( int i = 0; i < Model::Renderer::RenderEffectPresetLibrary::get().getPresetCount(); i++ )
		{
			const Model::Renderer::RenderEffectPreset & preset
				= *Model::Renderer::RenderEffectPresetLibrary::get().getPreset( i );

			QAction * const action = new QAction( QString::fromStdString( preset.getName() ), _renderSettingPreset );
			action->setData( i );
			action->setCheckable( true );
			_renderSettingPreset->addAction( action );
		}
		QAction * const openRenderEffectSettingsAction = new QAction( "Settings...", _renderSettingPreset );
		openRenderEffectSettingsAction->setData( -1 );
		_renderSettingPreset->addAction( openRenderEffectSettingsAction );

		addMenu( _renderSettingPreset );

		addAction( "Snapshot", this, &ContextualMenuRender::_takeSnapshotAction );
		addAction( "Export Image...", this, &ContextualMenuRender::_exportImageAction );

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

		addSection( "Measurement" );
		QMenu * const measurementModeMenu = new QMenu( this );
		measurementModeMenu->setTitle( "Measurement" );
		measurementModeMenu->addAction( "Distance" )
			->setProperty( MEASUREMENT_MODE_PROPERTY_NAME, int( Controller::MeasurementPicker::Mode::DISTANCE ) );
		// measurementModeMenu->addAction( "Distance to cycle" )
		//	->setProperty( MEASUREMENT_MODE_PROPERTY_NAME,
		//				   int( Controller::MeasurementPicker::Mode::DISTANCE_TO_CYCLE ) );
		measurementModeMenu->addAction( "Angle" )->setProperty( MEASUREMENT_MODE_PROPERTY_NAME,
																int( Controller::MeasurementPicker::Mode::ANGLE ) );
		measurementModeMenu->addAction( "Dihedral angle" )
			->setProperty( MEASUREMENT_MODE_PROPERTY_NAME, int( Controller::MeasurementPicker::Mode::DIHEDRAL_ANGLE ) );
		connect( measurementModeMenu, &QMenu::triggered, this, &ContextualMenuRender::_setMeasurementMode );

		addMenu( measurementModeMenu );

		connect( _backgroundColorMenu,
				 QOverload<QAction *>::of( &QMenu::triggered ),
				 this,
				 &ContextualMenuRender::_setBackgroundColorAction );
		connect( _projectionMenu,
				 QOverload<QAction *>::of( &QMenu::triggered ),
				 this,
				 &ContextualMenuRender::_changeProjectionAction );
		connect( _renderSettingPreset,
				 QOverload<QAction *>::of( &QMenu::triggered ),
				 this,
				 &ContextualMenuRender::_applyRenderEffectPresetAction );

		_refreshPickerMode();
		_refreshCameraProjection();
		_refreshAppliedRenderSettingPreset();
	}

	void ContextualMenuRender::localize() {}

	void ContextualMenuRender::_refreshPickerMode() const
	{
		const ID::VTX_ID & pickerID = VTXApp::get()
										  .getStateMachine()
										  .getState<State::Visualization>( ID::State::VISUALIZATION )
										  ->getCurrentPickerID();

		_selectionModeAction->setChecked( pickerID == ID::Controller::PICKER );
		_measurementModeAction->setChecked( pickerID == ID::Controller::MEASUREMENT );
	}

	void ContextualMenuRender::_refreshCameraProjection() const
	{
		const bool cameraProjectionIsPerspective = VTX_SETTING().getCameraPerspective();

		_projectionMenu->actions()[ 0 ]->setChecked( cameraProjectionIsPerspective );
		_projectionMenu->actions()[ 1 ]->setChecked( !cameraProjectionIsPerspective );
	}
	void ContextualMenuRender::_refreshAppliedRenderSettingPreset() const
	{
		const int appliedRenderSettingIndex = Model::Renderer::RenderEffectPresetLibrary::get().getAppliedPresetIndex();

		for ( int i = 0; i < Model::Renderer::RenderEffectPresetLibrary::get().getPresetCount(); i++ )
		{
			_renderSettingPreset->actions()[ i ]->setChecked( i == appliedRenderSettingIndex );
		}
	}

	void ContextualMenuRender::_loadMoleculeAction() const { UI::Dialog::openLoadMoleculeDialog(); }
	void ContextualMenuRender::_downloadMoleculeAction() const { UI::Dialog::openDownloadMoleculeDialog(); }
	void ContextualMenuRender::_showAllMoleculesAction() const { VTX_ACTION( new Action::Scene::ShowAllMolecules() ); }

	void ContextualMenuRender::_setPickerToSelection() const
	{
		VTX_ACTION( new Action::Main::ChangePicker( ID::Controller::PICKER ) );
	}
	void ContextualMenuRender::_setPickerToMeasurement() const
	{
		VTX_ACTION( new Action::Main::ChangePicker( ID::Controller::MEASUREMENT ) );
	}

	void ContextualMenuRender::_setSelectionGranularityAction( QAction * p_action ) const
	{
		const VTX::Selection::Granularity granularity
			= VTX::Selection::Granularity( p_action->property( SELECTION_GRANULARITY_PROPERTY_NAME ).toInt() );
		VTX_ACTION( new Action::Setting::ChangeSelectionGranularity( granularity ) );
	}

	void ContextualMenuRender::_setMeasurementMode( QAction * p_action ) const
	{
		const Controller::MeasurementPicker::Mode mode
			= Controller::MeasurementPicker::Mode( p_action->property( MEASUREMENT_MODE_PROPERTY_NAME ).toInt() );
		VTX_ACTION( new Action::Main::ChangePicker( ID::Controller::MEASUREMENT, int( mode ) ) );
	}
	void ContextualMenuRender::_changeProjectionAction( QAction * const p_action )
	{
		const Settings::VTXSettings::CameraProjection projection
			= Settings::VTXSettings::CameraProjection( p_action->data().toInt() );

		const bool changeToPerspective = projection == Settings::VTXSettings::CameraProjection::PERSPECTIVE;
		VTX_ACTION( new Action::Setting::ChangeCameraProjectionToPerspective( changeToPerspective ) );
	}
	void ContextualMenuRender::_setBackgroundColorAction( QAction * const p_action )
	{
		const Renderer::DEFAULT_BACKGROUND background = Renderer::DEFAULT_BACKGROUND( p_action->data().toInt() );

		VTX_ACTION( new Action::Renderer::ChangeBackgroundColor(
			VTX_RENDER_EFFECT(), Renderer::DEFAULT_BACKGROUND_COLORS[ int( background ) ] ) );
	}
	void ContextualMenuRender::_applyRenderEffectPresetAction( QAction * const p_action )
	{
		const int presetIndex = p_action->data().toInt();

		if ( presetIndex == -1 )
		{
			VTXApp::get().getMainWindow().openSettingWindow( UI::Widget::Settings::SETTING_MENU::RENDER_EFFECTS );
		}
		else
		{
			Model::Renderer::RenderEffectPreset * const preset
				= Model::Renderer::RenderEffectPresetLibrary::get().getPreset( presetIndex );
			VTX_ACTION( new Action::Renderer::ApplyRenderEffectPreset( *preset ) );
		}
	}
	void ContextualMenuRender::_takeSnapshotAction()
	{
		VTX_ACTION( new Action::Main::Snapshot( Worker::Snapshoter::MODE::GL,
												Util::Filesystem::getUniqueSnapshotsPath(),
												VTX_SETTING().getSnapshotResolution() ) );
	}
	void ContextualMenuRender::_exportImageAction() { Dialog::openAdvancedSettingImageExportDialog(); }

} // namespace VTX::UI::Widget::ContextualMenu
