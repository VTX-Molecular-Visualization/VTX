#include "ui/qt/tool/render/widget/render_widget.hpp"
#include "ui/qt/action/main.hpp"
#include "ui/qt/action/viewpoint.hpp"
#include "ui/qt/tool/render/widget/base_integrated_widget.hpp"
#include "ui/qt/tool/render/widget/overlay/visualization_quick_access.hpp"
#include "ui/qt/widget_factory.hpp"
#include <app/old/action/main.hpp>
#include <app/old/action/setting.hpp>
#include <app/old/action/viewpoint.hpp>
#include <app/old/event.hpp>
#include <app/old/internal/io/filesystem.hpp>
// #include <app/old/component/object3d/label.hpp>
// #include <app/model/measurement/angle.hpp>
// #include <app/model/measurement/dihedral_angle.hpp>
// #include <app/model/measurement/distance.hpp>
// #include <app/model/measurement/measure_in_progress.hpp>
#include <app/old/component/chemistry/molecule.hpp>
#include <app/old/component/object3d/mesh_triangle.hpp>
// #include "ui/state/state_machine.hpp"
// #include "ui/state/visualization.hpp"
#include "ui/qt/style.hpp"
#include <app/old/event/global.hpp>
#include <util/logger.hpp>
// #include "ui/view/ui/widget/measurement/angle_render_view.hpp"
// #include "ui/view/ui/widget/measurement/dihedral_angle_render_view.hpp"
// #include "ui/view/ui/widget/measurement/distance_render_view.hpp"
// #include "ui/view/ui/widget/measurement/measure_in_progress_render_view.hpp"
#include <QShortcut>
#include <app/old/vtx_app.hpp>

namespace VTX::UI::QT::Tool::Render::Widget
{
	RenderWidget::RenderWidget( QWidget * p_parent ) : QtPanelTemplate( p_parent )
	{
		name			  = "Render";
		defaultSize		  = Style::RENDER_PREFERRED_SIZE;
		visibleByDefault  = true;
		referenceInPanels = false;

		_registerEvent( VTX::App::Old::Event::Global::APPLIED_RENDER_EFFECT_CHANGE );
		_registerEvent( VTX::App::Old::Event::Global::LABEL_ADDED );
		_registerEvent( VTX::App::Old::Event::Global::LABEL_REMOVED );
		_registerEvent( VTX::App::Old::Event::Global::PICKER_MODE_CHANGE );
	}

	RenderWidget::~RenderWidget() {}

	void RenderWidget::receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Old::Event::Global::APPLIED_RENDER_EFFECT_CHANGE )
		{
			updateRenderSetting( App::Old::Render::Renderer::RENDER_SETTING::SHADING );
			updateRenderSetting( App::Old::Render::Renderer::RENDER_SETTING::SSAO );
			updateRenderSetting( App::Old::Render::Renderer::RENDER_SETTING::OUTLINE );
			updateRenderSetting( App::Old::Render::Renderer::RENDER_SETTING::FOG );
			updateRenderSetting( App::Old::Render::Renderer::RENDER_SETTING::AA );
		}

		// if ( p_event.name == VTX::App::Old::Event::Global::LABEL_ADDED )
		//{
		//	const App::Old::Core::Event::VTXEventArg<App::Old::Component::Object3D::Label*> & castedEvent
		//		= dynamic_cast<const App::Old::Core::Event::VTXEventArg<App::Old::Component::Object3D::Label*> &>( p_event );

		//	const App::Old::VTX_ID & labeltype = castedEvent.ptr->getTypeId();

		//	BaseIntegratedWidget * integratedWidget = nullptr;

		//	if ( labeltype == ID::Model::MODEL_MEASUREMENT_DISTANCE )
		//	{
		//		Model::Measurement::Distance * const distanceModel
		//			= static_cast<Model::Measurement::Distance *>( castedEvent.ptr );

		//		integratedWidget = WidgetFactory::get()
		//							   .instantiateViewWidget<View::UI::Widget::Measurement::DistanceRenderView,
		//													  Model::Measurement::Distance>(
		//								   distanceModel, ID::View::UI_RENDER_MEASUREMENT_DISTANCE, this, "Distance" );
		//	}
		//	else if ( labeltype == ID::Model::MODEL_MEASUREMENT_ANGLE )
		//	{
		//		Model::Measurement::Angle * const angleModel
		//			= static_cast<Model::Measurement::Angle *>( castedEvent.ptr );

		//		integratedWidget = WidgetFactory::get()
		//							   .instantiateViewWidget<View::UI::Widget::Measurement::AngleRenderView,
		//													  Model::Measurement::Angle>(
		//								   angleModel, ID::View::UI_RENDER_MEASUREMENT_ANGLE, this, "Angle" );
		//	}
		//	else if ( labeltype == ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE )
		//	{
		//		Model::Measurement::DihedralAngle * const dihedralAngleModel
		//			= static_cast<Model::Measurement::DihedralAngle *>( castedEvent.ptr );

		//		integratedWidget = WidgetFactory::get()
		//							   .instantiateViewWidget<View::UI::Widget::Measurement::DihedralAngleRenderView,
		//													  Model::Measurement::DihedralAngle>(
		//								   dihedralAngleModel,
		//								   ID::View::UI_RENDER_MEASUREMENT_DIHEDRAL_ANGLE,
		//								   this,
		//								   "DihedralAngle" );
		//	}

		//	if ( integratedWidget != nullptr )
		//	{
		//		_addIntegratedWidget( integratedWidget );
		//	}
		//}
		// else if ( p_event.name == VTX::App::Old::Event::Global::LABEL_REMOVED )
		//{
		//	const App::Old::Core::Event::VTXEventArg<App::Old::Component::Object3D::Label*> & castedEvent
		//		= dynamic_cast<const App::Old::Core::Event::VTXEventArg<App::Old::Component::Object3D::Label*> &>( p_event );

		//	const App::Old::VTX_ID & labelTypeID = castedEvent.ptr->getTypeId();

		//	if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_DISTANCE )
		//	{
		//		const Model::Measurement::Distance * const model
		//			= dynamic_cast<const Model::Measurement::Distance *>( castedEvent.ptr );
		//		_removeViewIntegratedWidget<View::UI::Widget::Measurement::DistanceRenderView,
		//									Model::Measurement::Distance>( model,
		//																   ID::View::UI_RENDER_MEASUREMENT_DISTANCE );
		//	}
		//	else if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_ANGLE )
		//	{
		//		const Model::Measurement::Angle * const model
		//			= dynamic_cast<const Model::Measurement::Angle *>( castedEvent.ptr );
		//		_removeViewIntegratedWidget<View::UI::Widget::Measurement::AngleRenderView, Model::Measurement::Angle>(
		//			model, ID::View::UI_RENDER_MEASUREMENT_ANGLE );
		//	}
		//	else if ( labelTypeID == ID::Model::MODEL_MEASUREMENT_DIHEDRAL_ANGLE )
		//	{
		//		const Model::Measurement::DihedralAngle * const model
		//			= dynamic_cast<const Model::Measurement::DihedralAngle *>( castedEvent.ptr );
		//		_removeViewIntegratedWidget<View::UI::Widget::Measurement::DihedralAngleRenderView,
		//									Model::Measurement::DihedralAngle>(
		//			model, ID::View::UI_RENDER_MEASUREMENT_DIHEDRAL_ANGLE );
		//	}
		//}
		// else if ( p_event.name == VTX::App::Old::Event::Global::PICKER_MODE_CHANGE )
		//{
		//	State::Visualization * const state
		//		= UI::VTXApp::get().getStateMachine().getState<State::Visualization>( ID::State::VISUALIZATION );

		//	Model::Measurement::MeasureInProgress & measureInProgressModel
		//		= state->getController<Controller::MeasurementPicker>( ID::Controller::MEASUREMENT )
		//			  ->getMeasureInProgressModel();

		//	if ( state->getCurrentPickerID() == ID::Controller::PICKER )
		//	{
		//		if ( VTX::MVC_MANAGER().hasView( &measureInProgressModel,
		//											 ID::View::UI_RENDER_MEASUREMENT_MEASURE_IN_PROGRESS ) )
		//		{
		//			_removeViewIntegratedWidget<View::UI::Widget::Measurement::MeasureInProgressRenderView,
		//										Model::Measurement::MeasureInProgress>(
		//				&measureInProgressModel, ID::View::UI_RENDER_MEASUREMENT_MEASURE_IN_PROGRESS );
		//		}
		//	}
		//	else if ( state->getCurrentPickerID() == ID::Controller::MEASUREMENT )
		//	{
		//		if ( !VTX::MVC_MANAGER().hasView( &measureInProgressModel,
		//											  ID::View::UI_RENDER_MEASUREMENT_MEASURE_IN_PROGRESS ) )
		//		{
		//			UI::Widget::Render::BaseIntegratedWidget * const measureInProgressView
		//				= WidgetFactory::get()
		//					  .instantiateViewWidget<View::UI::Widget::Measurement::MeasureInProgressRenderView,
		//											 Model::Measurement::MeasureInProgress>(
		//						  &measureInProgressModel,
		//						  ID::View::UI_RENDER_MEASUREMENT_MEASURE_IN_PROGRESS,
		//						  this,
		//						  "MeasureInProgress" );

		//			_addIntegratedWidget( measureInProgressView );
		//		}
		//	}
		//}
	}

	void RenderWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_openGLWidget = WidgetFactory::get().instantiateWidget<Widget::OpenGLWidget>( this, "openglWidget" );

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
		QShortcut * shortcut = new QShortcut( QKeySequence( tr( "F1" ) ), this );
		shortcut->setContext( Qt::WidgetWithChildrenShortcut );
		connect( shortcut, &QShortcut::activated, this, &RenderWidget::_onShortcutToggleCameraController );
		shortcut = new QShortcut( QKeySequence( tr( "Ctrl+F1" ) ), this );
		shortcut->setContext( Qt::WidgetWithChildrenShortcut );
		connect( shortcut, &QShortcut::activated, this, &RenderWidget::_onShortcutResetCameraController );
		shortcut = new QShortcut( QKeySequence( tr( "F2" ) ), this );
		shortcut->setContext( Qt::WidgetWithChildrenShortcut );
		connect( shortcut, &QShortcut::activated, this, &RenderWidget::_onShortcutAddViewpoint );
		shortcut = new QShortcut( QKeySequence( tr( "F5" ) ), this );
		shortcut->setContext( Qt::WidgetWithChildrenShortcut );
		connect( shortcut, &QShortcut::activated, this, &RenderWidget::_onShortcutSnapshot );
#ifndef VTX_PRODUCTION
		shortcut = new QShortcut( QKeySequence( tr( "F7" ) ), this );
		shortcut->setContext( Qt::WidgetWithChildrenShortcut );
		connect( shortcut, &QShortcut::activated, this, &RenderWidget::_onShortcutChangeRenderMode );
		shortcut = new QShortcut( QKeySequence( tr( "Space" ) ), this );
		shortcut->setContext( Qt::WidgetWithChildrenShortcut );
		connect( shortcut, &QShortcut::activated, this, &RenderWidget::_onShortcutPrintCameraInfos );
#endif
	}

	void RenderWidget::resizeEvent( QResizeEvent * p_event )
	{
		for ( const std::pair<const Overlay::OVERLAY, Overlay::BaseOverlay *> & overlay : _overlays )
		{
			overlay.second->updatePosition( p_event->size() );
		}
	}

	void RenderWidget::_onShortcutToggleCameraController()
	{
		VTX_ACTION( new QT::Action::Main::ToggleCameraController() );
	}

	void RenderWidget::_onShortcutResetCameraController()
	{
		VTX_ACTION( new QT::Action::Main::ResetCameraController() );
	}

	void RenderWidget::_onShortcutAddViewpoint() { VTX_ACTION( new QT::Action::Viewpoint::Create() ); }

	void RenderWidget::_onShortcutSnapshot()
	{
		VTX_ACTION( new VTX::App::Old::Action::Main::Snapshot(
			App::Old::Render::Worker::Snapshoter::MODE::GL,
			App::Old::Internal::IO::Filesystem::getUniqueSnapshotsPath( VTX_SETTING().getSnapshotFormat() ),
			VTX_SETTING().getSnapshotResolution() ) );
	}

	void RenderWidget::_onShortcutChangeRenderMode()
	{
		VTX_ACTION( new VTX::App::Old::Action::Setting::ChangeRenderMode( App::Old::Render::Renderer::MODE(
			( (uint)VTX_SETTING().mode + 1 ) % (uint)App::Old::Render::Renderer::MODE::COUNT ) ) );
	}

	void RenderWidget::_onShortcutPrintCameraInfos() { App::Old::VTXApp::get().getScene().getCamera().print(); }

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

	void RenderWidget::updateRenderSetting( const App::Old::Render::Renderer::RENDER_SETTING p_setting )
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

		if ( p_anchor == Overlay::OVERLAY_ANCHOR::BOTTOM_CENTER )
			overlay->setFixedHeight( 32 );

		overlay->updatePosition( contentsRect().size() );
	}
	void RenderWidget::hideOverlay( const Overlay::OVERLAY & p_overlay )
	{
		_overlays[ p_overlay ]->setVisible( false );
	}

	Overlay::BaseOverlay * RenderWidget::_instantiateOverlay( const Overlay::OVERLAY & p_overlayType )
	{
		Widget::Overlay::BaseOverlay * res;

		switch ( p_overlayType )
		{
		case Overlay::OVERLAY::VISUALIZATION_QUICK_ACCESS:
			res = QT::WidgetFactory::get().instantiateWidget<Widget::Overlay::VisualizationQuickAccess>( this, "" );
			break;

		default:
			VTX_ERROR( "Overlay {} not managed in RenderWidget::InstantiatedOverlay::instantiateOverlay.",
					   int( p_overlayType ) );
			res = nullptr;
			break;
		}

		return res;
	}
} // namespace VTX::UI::QT::Tool::Render::Widget
