#include "render_widget.hpp"
#include "action/main.hpp"
#include "action/viewpoint.hpp"
#include "event/event_manager.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "overlay/visualization_quick_access.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "style.hpp"
#include "tool/logger.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"
#include <QShortcut>

namespace VTX::UI::Widget::Render
{
	RenderWidget::RenderWidget( QWidget * p_parent ) : BaseManualWidget<QWidget>( p_parent )
	{
		_registerEvent( Event::Global::MOLECULE_CREATED );
		_registerEvent( Event::Global::MESH_CREATED );
	}

	RenderWidget::~RenderWidget() {}

	void RenderWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		_openGLWidget->makeCurrent();
		if ( p_event.name == Event::Global::MOLECULE_CREATED )
		{
			const Event::VTXEventPtr<Model::Molecule> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
			castedEvent.ptr->init();
		}
		else if ( p_event.name == Event::Global::MESH_CREATED )
		{
			const Event::VTXEventPtr<Model::MeshTriangle> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::MeshTriangle> &>( p_event );
			castedEvent.ptr->init();
		}
		_openGLWidget->doneCurrent();
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

	void RenderWidget::localize()
	{
		setWindowTitle( "Render" );
		// setWindowTitle( QCoreApplication::translate( "RenderWidget", "Render", nullptr ) );
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
		Overlay::BaseOverlay * res;

		switch ( p_overlayType )
		{
		case Overlay::OVERLAY::VISUALIZATION_QUICK_ACCESS:
			res = WidgetFactory::get().instantiateWidget<Overlay::VisualizationQuickAccess>( this, "" );
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
