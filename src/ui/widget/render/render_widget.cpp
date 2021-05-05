#include "render_widget.hpp"
#include "event/event_manager.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "style.hpp"

namespace VTX::UI::Widget::Render
{
	RenderWidget::RenderWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
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
			castedEvent.ptr->init( getOpenGLWidget().getGL() );
		}
		else if ( p_event.name == Event::Global::MESH_CREATED )
		{
			const Event::VTXEventPtr<Model::MeshTriangle> & castedEvent
				= dynamic_cast<const Event::VTXEventPtr<Model::MeshTriangle> &>( p_event );
			castedEvent.ptr->init( getOpenGLWidget().getGL() );
		}
		_openGLWidget->doneCurrent();
	}

	void RenderWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_openGLWidget->setSizeHint( Style::RENDER_PREFERED_SIZE );
		_openGLWidget->setMinimumSizeHint( Style::RENDER_MINIMUM_SIZE );

		setFocusPolicy( Qt::StrongFocus );
		setFeatures( DockWidgetFeature::DockWidgetClosable );
		setWidget( _openGLWidget );

		_openGLWidget->setMinimumSize( Style::RENDER_MINIMUM_SIZE );

		QSizePolicy sizePolicy = QSizePolicy(
			QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred, QSizePolicy::ControlType::Frame );
		sizePolicy.setHorizontalStretch( 30 );
		sizePolicy.setVerticalStretch( 30 );

		_openGLWidget->setSizePolicy( sizePolicy );
	}

	void RenderWidget::_setupSlots() {}

	void RenderWidget::localize()
	{
		setWindowTitle( "Render" );
		// setWindowTitle( QCoreApplication::translate( "RenderWidget", "Render", nullptr ) );
	}

	void RenderWidget::mouseMoveEvent( QMouseEvent * p_event )
	{
		Event::EventManager::get().fireEventMouse( p_event );
		p_event->accept();
	}

	void RenderWidget::mousePressEvent( QMouseEvent * p_event )
	{
		Event::EventManager::get().fireEventMouse( p_event );
		p_event->accept();
	}

	void RenderWidget::mouseReleaseEvent( QMouseEvent * p_event )
	{
		Event::EventManager::get().fireEventMouse( p_event );
		p_event->accept();
	}

	void RenderWidget::keyPressEvent( QKeyEvent * p_event )
	{
		Event::EventManager::get().fireEventKeyboard( p_event );
		p_event->accept();
	}

	void RenderWidget::keyReleaseEvent( QKeyEvent * p_event )
	{
		Event::EventManager::get().fireEventKeyboard( p_event );
		p_event->accept();
	}

	void RenderWidget::wheelEvent( QWheelEvent * p_event )
	{
		Event::EventManager::get().fireEventWheel( p_event );
		p_event->accept();
	}

} // namespace VTX::UI::Widget::Render
