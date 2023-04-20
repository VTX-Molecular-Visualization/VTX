#include "ui/old_ui/ui/widget/contextual_menu/contextual_menu_render.hpp"
// #include "ui/old_ui/controller/measurement_picker.hpp"
#include "ui/old_ui/action/main.hpp"
#include "ui/old_ui/state/visualization.hpp"
#include "ui/old_ui/ui/dialog.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/ui/widget/renderer/default_background.hpp"
#include "ui/old_ui/ui/widget/settings/setting_widget_enum.hpp"
#include "ui/old_ui/util/ui.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include "ui/qt/action/main.hpp"
#include <app/core/action/action_manager.hpp>
#include <app/action/main.hpp>
#include <app/action/renderer.hpp>
#include <app/action/scene.hpp>
#include <app/action/setting.hpp>
#include <app/action/viewpoint.hpp>
#include <app/old_app/io/filesystem.hpp>
#include <app/model/renderer/render_effect_preset_library.hpp>
#include <app/old_app/object3d/scene.hpp>
#include <app/old_app/setting.hpp>
#include <app/worker/snapshoter.hpp>

namespace VTX::UI::Widget::ContextualMenu
{
	ContextualMenuRender::ContextualMenuRender( QWidget * const p_parent ) : BaseContextualMenu( p_parent )
	{
		_registerEvent( VTX::Event::Global::PICKER_MODE_CHANGE );
		_registerEvent( VTX::Event::Global::SETTINGS_CHANGE );
		_registerEvent( VTX::Event::Global::APPLIED_RENDER_EFFECT_CHANGE );
		_registerEvent( VTX::Event::Global::RENDER_OVERLAY_VISIBILITY_CHANGE );
	}
	ContextualMenuRender ::~ContextualMenuRender() {}

	void ContextualMenuRender ::receiveEvent( const VTX::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::Event::Global::PICKER_MODE_CHANGE )
		{
			_refreshPickerMode();
		}
		else if ( p_event.name == VTX::Event::Global::APPLIED_RENDER_EFFECT_CHANGE )
		{
			_refreshAppliedRenderSettingPreset();
		}
		else if ( p_event.name == VTX::Event::Global::SETTINGS_CHANGE )
		{
			const VTX::Event::VTXEventRef<std::set<Setting::PARAMETER>> & castedEvent
				= dynamic_cast<const VTX::Event::VTXEventRef<std::set<Setting::PARAMETER>> &>( p_event );

			if ( castedEvent.ref.find( Setting::PARAMETER::CAMERA_PROJECTION ) != castedEvent.ref.end() )
			{
				_refreshCameraProjection();
			}

			if ( castedEvent.ref.find( Setting::PARAMETER::SELECTION_GRANULARITY ) != castedEvent.ref.end() )
			{
				_refreshSelectionGranularityMenu();
			}
		}
		else if ( p_event.name == VTX::Event::Global::RENDER_OVERLAY_VISIBILITY_CHANGE )
		{
			_refreshOverlayVisibilityMenu();
		}
	}

	void ContextualMenuRender::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		addSection( "Loading" );
		addAction( "Load Molecule", this, &ContextualMenuRender::_loadMoleculeAction );
		addAction( "Download Molecule", this, &ContextualMenuRender::_downloadMoleculeAction );

		addSection( "Mouse Mode" );
		_selectionModeAction = addAction( "Selection", this, &ContextualMenuRender::_setPickerToSelection );
		_selectionModeAction->setCheckable( true );

		//_measurementModeMenu = new QMenu( this );
		//_measurementModeMenu->setTitle( "Measurement" );
		//_addMeasurementModeActionInMenu( int( Controller::MeasurementPicker::Mode::DISTANCE ), "Distance" );
		////_addMeasurementModeActionInMenu( int( Controller::MeasurementPicker::Mode::DISTANCE_TO_CYCLE ),
		////								 "Distance to cycle" );
		//_addMeasurementModeActionInMenu( int( Controller::MeasurementPicker::Mode::ANGLE ), "Angle" );
		//_addMeasurementModeActionInMenu( int( Controller::MeasurementPicker::Mode::DIHEDRAL_ANGLE ), "Dihedral angle"
		//); _measurementModeAction = addMenu( _measurementModeMenu ); _measurementModeAction->setCheckable( true );

		addSection( "Selection" );
		_selectionGranularityMenu = new QMenu( this );
		_selectionGranularityMenu->setTitle( "Selection target" );
		_addSelectionGranularityActionInMenu( int( VTX::Selection::Granularity::ATOM ), "Atom" );
		_addSelectionGranularityActionInMenu( int( VTX::Selection::Granularity::RESIDUE ), "Residue" );
		_addSelectionGranularityActionInMenu( int( VTX::Selection::Granularity::CHAIN ), "Chain" );
		_addSelectionGranularityActionInMenu( int( VTX::Selection::Granularity::MOLECULE ), "Molecule" );
		addMenu( _selectionGranularityMenu );

		addSection( "Render" );
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

		addSection( "Action" );
		addAction( "Reset Camera", this, &ContextualMenuRender::_resetCameraAction );
		addAction( "Show All", this, &ContextualMenuRender::_showAllMoleculesAction );

		addSection( "Overlays" );
		_overlaysMenu = new QMenu( "Overlays", this );

		QAction * const actionShowAllOverlays = new QAction( "Show all", _overlaysMenu );
		actionShowAllOverlays->setData( SHOW_ALL_OVERLAYS_DATA_VALUE );
		_overlaysMenu->addAction( actionShowAllOverlays );

		QAction * const actionHideAllOverlays = new QAction( "Hide all", _overlaysMenu );
		actionHideAllOverlays->setData( HIDE_ALL_OVERLAYS_DATA_VALUE );
		_overlaysMenu->addAction( actionHideAllOverlays );
		_overlaysMenu->addSeparator();
		Util::UI::fillMenu( *_overlaysMenu,
							int( Widget::Render::Overlay::OVERLAY::COUNT ),
							Widget::Render::Overlay::OVERLAY_NAMES_STR,
							true );
		addMenu( _overlaysMenu );

		addSection( "Image" );
		addAction( "Snapshot", this, &ContextualMenuRender::_takeSnapshotAction );
		addAction( "Export Image...", this, &ContextualMenuRender::_exportImageAction );

		_refreshPickerMode();
		_refreshSelectionGranularityMenu();
		_refreshCameraProjection();
		_refreshAppliedRenderSettingPreset();
		_refreshOverlayVisibilityMenu();
	}

	void ContextualMenuRender::_addSelectionGranularityActionInMenu( const int		 p_granularity,
																	 const QString & p_name ) const
	{
		QAction * const action = _selectionGranularityMenu->addAction( p_name );
		action->setCheckable( true );
		action->setData( p_granularity );
	}
	void ContextualMenuRender::_addMeasurementModeActionInMenu( const int p_mode, const QString & p_name ) const
	{
		QAction * const action = _measurementModeMenu->addAction( p_name );
		action->setCheckable( true );
		action->setData( p_mode );
	}

	void ContextualMenuRender::_setupSlots()
	{
		connect( _backgroundColorMenu, &QMenu::triggered, this, &ContextualMenuRender::_setBackgroundColorAction );
		connect( _overlaysMenu, &QMenu::triggered, this, &ContextualMenuRender::_setOverlayVisibilityAction );

		connect( _projectionMenu, &QMenu::triggered, this, &ContextualMenuRender::_changeProjectionAction );
		connect( _renderSettingPreset, &QMenu::triggered, this, &ContextualMenuRender::_applyRenderEffectPresetAction );
		connect(
			_selectionGranularityMenu, &QMenu::triggered, this, &ContextualMenuRender::_setSelectionGranularityAction );
		connect( _measurementModeMenu, &QMenu::triggered, this, &ContextualMenuRender::_setMeasurementMode );
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

		_refreshMeasurementModeMenu();
	}

	void ContextualMenuRender::_refreshSelectionGranularityMenu() const
	{
		const State::Visualization * const state
			= VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );
		const VTX::Selection::Granularity currentGranularity = VTX_SETTING().getSelectionGranularity();

		for ( QAction * const action : _selectionGranularityMenu->actions() )
		{
			const VTX::Selection::Granularity granularity = VTX::Selection::Granularity( action->data().toInt() );
			action->setChecked( currentGranularity == granularity );
		}
	}
	void ContextualMenuRender::_refreshMeasurementModeMenu() const
	{
		const State::Visualization * const state
			= VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		const ID::VTX_ID & pickerID = state->getCurrentPickerID();

		// const Controller::MeasurementPicker::Mode & currentMode
		//	= state->getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT )->getCurrentMode();

		// for ( QAction * const action : _measurementModeMenu->actions() )
		//{
		//	const Controller::MeasurementPicker::Mode mode
		//		= Controller::MeasurementPicker::Mode( action->data().toInt() );

		//	const bool checked = pickerID == ID::Controller::MEASUREMENT && currentMode == mode;

		//	action->setChecked( checked );
		//}
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
	void ContextualMenuRender::_refreshOverlayVisibilityMenu() const
	{
		const Render::RenderWidget & renderWigdet
			= VTXApp::get().getMainWindow().getWidget<Render::RenderWidget>( ID::UI::Window::RENDER );

		for ( QAction * const action : _overlaysMenu->actions() )
		{
			const int actionData = action->data().toInt();

			if ( actionData >= 0 )
			{
				const Render::Overlay::OVERLAY overlay	  = Render::Overlay::OVERLAY( actionData );
				const bool					   checkState = renderWigdet.getOverlay( overlay )->isVisible();
				action->setChecked( checkState );
			}
		}
	}

	void ContextualMenuRender::_loadMoleculeAction() const { UI::Dialog::openLoadMoleculeDialog(); }
	void ContextualMenuRender::_downloadMoleculeAction() const { UI::Dialog::openDownloadMoleculeDialog(); }
	void ContextualMenuRender::_showAllMoleculesAction() const { VTX_ACTION( new Action::Scene::ShowAllMolecules() ); }
	void ContextualMenuRender::_resetCameraAction() const
	{
		VTX_ACTION( new QT::Action::Main::ResetCameraController() );
	}

	void ContextualMenuRender::_setPickerToSelection() const
	{
		const State::Visualization * const state
			= VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		if ( state->getCurrentPickerID() != ID::Controller::PICKER )
			VTX_ACTION( new Action::Main::ChangePicker( ID::Controller::PICKER ) );
		else
			_refreshPickerMode();
	}

	void ContextualMenuRender::_setSelectionGranularityAction( QAction * p_action ) const
	{
		const VTX::Selection::Granularity granularity = VTX::Selection::Granularity( p_action->data().toInt() );
		VTX_ACTION( new Action::Main::ChangeSelectionGranularity( granularity ) );
	}

	void ContextualMenuRender::_setMeasurementMode( QAction * p_action ) const
	{
		// const Controller::MeasurementPicker::Mode mode
		//	= Controller::MeasurementPicker::Mode( p_action->data().toInt() );
		// VTX_ACTION( new Action::Main::ChangePicker( ID::Controller::MEASUREMENT, int( mode ) ) );
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
	void ContextualMenuRender::_setOverlayVisibilityAction( QAction * const p_action )
	{
		Widget::Render::RenderWidget & renderWidget
			= VTXApp::get().getMainWindow().getWidget<Widget::Render::RenderWidget>( ID::UI::Window::RENDER );

		const int actionData = p_action->data().toInt();

		if ( actionData == SHOW_ALL_OVERLAYS_DATA_VALUE || actionData == HIDE_ALL_OVERLAYS_DATA_VALUE )
		{
			const bool newVisibleState = actionData == SHOW_ALL_OVERLAYS_DATA_VALUE;
			renderWidget.showAllOverlays( newVisibleState );
		}
		else
		{
			const Render::Overlay::OVERLAY				 overlayEnum   = Widget::Render::Overlay::OVERLAY( actionData );
			Widget::Render::Overlay::BaseOverlay * const overlayWidget = renderWidget.getOverlay( overlayEnum );
			const bool									 newVisibleState = !overlayWidget->isVisible();

			renderWidget.setOverlayVisibility( overlayEnum, newVisibleState );
		}
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
		VTX_ACTION(
			new Action::Main::Snapshot( Worker::Snapshoter::MODE::GL,
										IO::Filesystem::getUniqueSnapshotsPath( VTX_SETTING().getSnapshotFormat() ),
										VTX_SETTING().getSnapshotResolution() ) );
	}
	void ContextualMenuRender::_exportImageAction() { Dialog::openAdvancedSettingImageExportDialog(); }

} // namespace VTX::UI::Widget::ContextualMenu
