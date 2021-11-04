#include "render_widget.hpp"
#include "controller/shortcut.hpp"
#include "event/event_manager.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"
#include "state/state_machine.hpp"
#include "state/visualization.hpp"
#include "style.hpp"
#include "vtx_app.hpp"

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

		_openGLWidget = new OpenGLWidget( this );

		setFocusPolicy( Qt::StrongFocus );

		QVBoxLayout * const layout = new QVBoxLayout( this );
		layout->setContentsMargins( 1, 1, 1, 1 );

		_openGLWidget->setSizePolicy( QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred );

		layout->addWidget( _openGLWidget );
	}

	void RenderWidget::_setupSlots() {}

	void RenderWidget::focusInEvent( QFocusEvent * p_event )
	{
		VTXApp::get()
			.getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->getController<Controller::Shortcut>( ID::Controller::SHORTCUT )
			->setGroup( Controller::SHORTCUTGROUP::RENDER );
	}
	void RenderWidget::focusOutEvent( QFocusEvent * p_event )
	{
		VTXApp::get()
			.getStateMachine()
			.getState<State::Visualization>( ID::State::VISUALIZATION )
			->getController<Controller::Shortcut>( ID::Controller::SHORTCUT )
			->setGroup( Controller::SHORTCUTGROUP::DEFAULT );
	}

	void RenderWidget::localize()
	{
		setWindowTitle( "Render" );
		// setWindowTitle( QCoreApplication::translate( "RenderWidget", "Render", nullptr ) );
	}
} // namespace VTX::UI::Widget::Render
