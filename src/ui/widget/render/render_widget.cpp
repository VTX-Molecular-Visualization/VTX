#include "render_widget.hpp"
#include "event/event_manager.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "style.hpp"

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

		_openGLWidget = new CustomWidget::DockWindowMainWidget<OpenGLWidget>(
			Style::RENDER_PREFERRED_SIZE, Style::RENDER_MINIMUM_SIZE, this );

		setFocusPolicy( Qt::StrongFocus );

		QVBoxLayout * const layout = new QVBoxLayout( this );
		layout->setContentsMargins( 1, 1, 1, 1);

		_openGLWidget->setSizePolicy( QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred );

		layout->addWidget( _openGLWidget );
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
