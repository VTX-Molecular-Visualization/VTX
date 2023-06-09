#include "render_widget.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "action/viewpoint.hpp"
#include "base_integrated_widget.hpp"
#include "event/event_manager.hpp"
#include "model/label.hpp"
#include "model/measurement/angle.hpp"
#include "model/measurement/dihedral_angle.hpp"
#include "model/measurement/distance.hpp"
#include "model/measurement/measure_in_progress.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "overlay/camera_quick_access.hpp"
#include "overlay/visualization_quick_access.hpp"
#include "setting.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "style.hpp"
#include "tool/logger.hpp"
#include "ui/shortcut.hpp"
#include "ui/widget_factory.hpp"
#include "view/ui/widget/measurement/angle_render_view.hpp"
#include "view/ui/widget/measurement/dihedral_angle_render_view.hpp"
#include "view/ui/widget/measurement/distance_render_view.hpp"
#include "view/ui/widget/measurement/measure_in_progress_render_view.hpp"
#include "vtx_app.hpp"
#include <QShortcut>

namespace VTX::UI::Widget::Render
{
	RenderWidget::RenderWidget( QWidget * p_parent ) : BaseManualWidget<QWidget>( p_parent )
	{
		_registerEvent( Event::Global::LABEL_ADDED );
		_registerEvent( Event::Global::LABEL_REMOVED );
		_registerEvent( Event::Global::PICKER_MODE_CHANGE );
	}

	RenderWidget::~RenderWidget() {}

	void RenderWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::LABEL_ADDED )
		{
			const Event::VTXEventPtr<Model::Label> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Label> &>( p_event );

			const ID::VTX_ID & labeltype = castedEvent.ptr->getTypeId();

			BaseIntegratedWidget * integratedWidget = nullptr;

			if ( labeltype == ID::Model::MODEL_MEASUREMENT_DISTANCE )
			{
				Model::Measurement::Distance * const distanceModel
					= static_cast<Model::Measurement::Distance *>( castedEvent.ptr );

				integratedWidget = WidgetFactory::get()
									   .instantiateViewWidget<View::UI::Widget::Measurement::DistanceRenderView,
															  Model::Measurement::Distance>(
										   distanceModel, ID::View::UI_RENDER_MEASUREMENT_DISTANCE, this, "Distance" );
			}
			else if ( labeltype == ID::Model::MODEL_MEASUREMENT_ANGLE )
			{
				Model::Measurement::Angle * const angleModel
					= static_cast<Model::Measurement::Angle *>( castedEvent.ptr );

				integratedWidget = WidgetFactory::get()
									   .instantiateViewWidget<View::UI::Widget::Measurement::AngleRenderView,
															  Model::Measurement::Angle>(
										   angleModel, ID::View::UI_RENDER_MEASUREMENT_ANGLE, this, "Angle" );
			}
			else if ( labeltype == ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE )
			{
				Model::Measurement::DihedralAngle * const dihedralAngleModel
					= static_cast<Model::Measurement::DihedralAngle *>( castedEvent.ptr );

				integratedWidget = WidgetFactory::get()
									   .instantiateViewWidget<View::UI::Widget::Measurement::DihedralAngleRenderView,
															  Model::Measurement::DihedralAngle>(
										   dihedralAngleModel,
										   ID::View::UI_RENDER_MEASUREMENT_DIHEDRAL_ANGLE,
										   this,
										   "DihedralAngle" );
			}

			if ( integratedWidget != nullptr )
			{
				_addIntegratedWidget( integratedWidget );
			}
		}
		else if ( p_event.name == Event::Global::LABEL_REMOVED )
		{
			const Event::VTXEventPtr<Model::Label> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Label> &>( p_event );

			const ID::VTX_ID & labelTypeID = castedEvent.ptr->getTypeId();

			if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_DISTANCE )
			{
				const Model::Measurement::Distance * const model
					= dynamic_cast<const Model::Measurement::Distance *>( castedEvent.ptr );
				_removeViewIntegratedWidget<View::UI::Widget::Measurement::DistanceRenderView,
											Model::Measurement::Distance>( model,
																		   ID::View::UI_RENDER_MEASUREMENT_DISTANCE );
			}
			else if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_ANGLE )
			{
				const Model::Measurement::Angle * const model
					= dynamic_cast<const Model::Measurement::Angle *>( castedEvent.ptr );
				_removeViewIntegratedWidget<View::UI::Widget::Measurement::AngleRenderView, Model::Measurement::Angle>(
					model, ID::View::UI_RENDER_MEASUREMENT_ANGLE );
			}
			else if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE )
			{
				const Model::Measurement::DihedralAngle * const model
					= dynamic_cast<const Model::Measurement::DihedralAngle *>( castedEvent.ptr );
				_removeViewIntegratedWidget<View::UI::Widget::Measurement::DihedralAngleRenderView,
											Model::Measurement::DihedralAngle>(
					model, ID::View::UI_RENDER_MEASUREMENT_DIHEDRAL_ANGLE );
			}
		}
		else if ( p_event.name == Event::Global::PICKER_MODE_CHANGE )
		{
			State::Visualization * const state
				= VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

			Model::Measurement::MeasureInProgress & measureInProgressModel
				= state->getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT )
					  ->getMeasureInProgressModel();

			if ( state->getCurrentPickerID() == ID::Controller::PICKER )
			{
				if ( MVC::MvcManager::get().hasView( &measureInProgressModel,
													 ID::View::UI_RENDER_MEASUREMENT_MEASURE_IN_PROGRESS ) )
				{
					_removeViewIntegratedWidget<View::UI::Widget::Measurement::MeasureInProgressRenderView,
												Model::Measurement::MeasureInProgress>(
						&measureInProgressModel, ID::View::UI_RENDER_MEASUREMENT_MEASURE_IN_PROGRESS );
				}
			}
			else if ( state->getCurrentPickerID() == ID::Controller::MEASUREMENT )
			{
				if ( !MVC::MvcManager::get().hasView( &measureInProgressModel,
													  ID::View::UI_RENDER_MEASUREMENT_MEASURE_IN_PROGRESS ) )
				{
					UI::Widget::Render::BaseIntegratedWidget * const measureInProgressView
						= WidgetFactory::get()
							  .instantiateViewWidget<View::UI::Widget::Measurement::MeasureInProgressRenderView,
													 Model::Measurement::MeasureInProgress>(
								  &measureInProgressModel,
								  ID::View::UI_RENDER_MEASUREMENT_MEASURE_IN_PROGRESS,
								  this,
								  "MeasureInProgress" );

					_addIntegratedWidget( measureInProgressView );
				}
			}
		}
	}

	void RenderWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_openGLWidget = WidgetFactory::get().instantiateWidget<Widget::Render::OpenGLWidget>( this, "openglWidget" );

		setFocusPolicy( Qt::StrongFocus );

		QVBoxLayout * const layout = new QVBoxLayout( this );
		layout->setContentsMargins( 1, 1, 1, 1 );

		_openGLWidget->setSizePolicy( QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred );

		layout->addWidget( _openGLWidget );
		setMouseTracking( true );
		_openGLWidget->setMouseTracking( true );
	}

	void RenderWidget::_setupSlots()
	{
		Shortcut::createLocal(
			Shortcut::Render::TOGGLE_CAMERA_CONTROLLER, this, &RenderWidget::_onShortcutToggleCameraController );

		Shortcut::createLocal(
			Shortcut::Render::RESET_CAMERA_CONTROLLER, this, &RenderWidget::_onShortcutResetCameraController );

		Shortcut::createLocal( Shortcut::Render::TOGGLE_CAMERA, this, &RenderWidget::_onShortcutToggleCamera );

		Shortcut::createLocal( Shortcut::Render::ADD_VIEWPOINT, this, &RenderWidget::_onShortcutAddViewpoint );

		Shortcut::createLocal( Shortcut::Render::SNAPSHOT, this, &RenderWidget::_onShortcutSnapshot );
		Shortcut::createLocal(
			Shortcut::Render::TOGGLE_ALL_OVERLAYS, this, &RenderWidget::_onShortcutToggleAllOverlays );

#ifndef VTX_PRODUCTION
		Shortcut::createLocal( Shortcut::Dev::CHANGE_RENDER_MODE, this, &RenderWidget::_onShortcutChangeRenderMode );

		Shortcut::createLocal( Shortcut::Dev::PRINT_CAMERA_INFOS, this, &RenderWidget::_onShortcutPrintCameraInfos );
#endif
	}

	void RenderWidget::resizeEvent( QResizeEvent * p_event )
	{
		for ( const std::pair<const Overlay::OVERLAY, Overlay::BaseOverlay *> & overlay : _overlays )
		{
			overlay.second->updatePosition( p_event->size() );
		}
	}

	void RenderWidget::_onShortcutToggleCameraController() { VTX_ACTION( new Action::Main::ToggleCameraController() ); }

	void RenderWidget::_onShortcutResetCameraController() { VTX_ACTION( new Action::Main::ResetCameraController() ); }

	void RenderWidget::_onShortcutToggleCamera()
	{
		const bool changeToPerspective = !VTX_SETTING().getCameraPerspective();
		VTX_ACTION( new Action::Setting::ChangeCameraProjectionToPerspective( changeToPerspective ) );
	}

	void RenderWidget::_onShortcutAddViewpoint() { VTX_ACTION( new Action::Viewpoint::Create() ); }

	void RenderWidget::_onShortcutSnapshot()
	{
		VTX_ACTION( new Action::Main::Snapshot( Worker::Snapshoter::MODE::GL,
												Util::Filesystem::getUniqueSnapshotsPath(),
												VTX_SETTING().getSnapshotResolution() ) );
	}

	void RenderWidget::_onShortcutChangeRenderMode()
	{
		VTX_ACTION( new Action::Setting::ChangeRenderMode(
			Renderer::MODE( ( (uint)VTX_SETTING().mode + 1 ) % (uint)Renderer::MODE::COUNT ) ) );
	}

	void RenderWidget::_onShortcutPrintCameraInfos() { VTXApp::get().getScene().getCamera().print(); }

	void RenderWidget::_onShortcutToggleAllOverlays()
	{
		bool visibleState = true;
		for ( auto pairIdOverlay : _overlays )
		{
			if ( pairIdOverlay.second->isVisible() )
			{
				visibleState = false;
				break;
			}
		}

		showAllOverlays( visibleState );
	}

	void RenderWidget::localize()
	{
		setWindowTitle( "Render" );
		// setWindowTitle( QCoreApplication::translate( "RenderWidget", "Render", nullptr ) );
	}

	void RenderWidget::_addIntegratedWidget( BaseIntegratedWidget * const p_widget )
	{
		_integratedWidgets.emplace_back( p_widget );
	}

	void RenderWidget::updateRender() const
	{
		_openGLWidget->update();
		for ( BaseIntegratedWidget * const integratedWidget : _integratedWidgets )
			integratedWidget->updatePosition();
	}

	void RenderWidget::updateRenderSetting( const Renderer::RENDER_SETTING p_setting )
	{
		_openGLWidget->updateRenderSetting( p_setting );
	}

	const Vec2i RenderWidget::getPickedIds( const uint p_x, const uint p_y )
	{
		return _openGLWidget->getPickedIds( p_x, p_y );
	}

	void RenderWidget::displayOverlay( const Overlay::OVERLAY &		   p_overlayType,
									   const Overlay::OVERLAY_ANCHOR & p_anchor )
	{
		std::map<Overlay::OVERLAY, Overlay::BaseOverlay *>::iterator itOverlay = _overlays.find( p_overlayType );

		Overlay::BaseOverlay * overlay;

		if ( itOverlay == _overlays.end() )
		{
			overlay = _instantiateOverlay( p_overlayType );
			overlay->setParent( this );
			_overlays.emplace( p_overlayType, overlay );
		}
		else
		{
			overlay = itOverlay->second;
		}

		overlay->setAnchorPosition( p_anchor );

		overlay->updatePosition( contentsRect().size() );
	}
	void RenderWidget::setOverlayVisibility( const Overlay::OVERLAY & p_overlay, const bool p_visible )
	{
		_overlays[ p_overlay ]->setVisible( p_visible );
		VTX_EVENT( new Event::VTXEvent( Event::Global::RENDER_OVERLAY_VISIBILITY_CHANGE ) );
	}
	void RenderWidget::showAllOverlays( const bool p_show )
	{
		for ( auto pairIdOverlay : _overlays )
		{
			pairIdOverlay.second->setVisible( p_show );
		}

		VTX_EVENT( new Event::VTXEvent( Event::Global::RENDER_OVERLAY_VISIBILITY_CHANGE ) );
	}

	Overlay::BaseOverlay * RenderWidget::getOverlay( const Overlay::OVERLAY & p_overlay )
	{
		auto mapIt = _overlays.find( p_overlay );

		if ( mapIt == _overlays.end() )
		{
			return nullptr;
		}

		return mapIt->second;
	}
	const Overlay::BaseOverlay * RenderWidget::getOverlay( const Overlay::OVERLAY & p_overlay ) const
	{
		auto mapIt = _overlays.find( p_overlay );

		if ( mapIt == _overlays.end() )
		{
			return nullptr;
		}

		return mapIt->second;
	}

	Overlay::BaseOverlay * RenderWidget::_instantiateOverlay( const Overlay::OVERLAY & p_overlayType )
	{
		Overlay::BaseOverlay * res;

		switch ( p_overlayType )
		{
		case Overlay::OVERLAY::VISUALIZATION_QUICK_ACCESS:
			res = WidgetFactory::get().instantiateWidget<Overlay::VisualizationQuickAccess>( this, "" );
			break;

		case Overlay::OVERLAY::CAMERA_PROJECTION_QUICK_ACCESS:
			res = WidgetFactory::get().instantiateWidget<Overlay::CameraQuickAccess>( this, "" );
			break;

		default:
			VTX_ERROR( "Overlay " + std::to_string( int( p_overlayType ) )
					   + " not managed in RenderWidget::InstantiatedOverlay::instantiateOverlay." );
			res = nullptr;
			break;
		}

		return res;
	}
} // namespace VTX::UI::Widget::Render
