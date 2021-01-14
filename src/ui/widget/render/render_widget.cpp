#include "render_widget.hpp"
#include "event/event_manager.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Render
			{
				RenderWidget::RenderWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
				{
					_registerEvent( Event::Global::MOLECULE_CREATED );
					_registerEvent( Event::Global::MESH_CREATED );
				}

				RenderWidget::~RenderWidget()
				{
					delete _openGLWidget;
					delete _verticalLayout;
					delete _verticalLayoutWidget;
				}

				void RenderWidget::receiveEvent( const Event::VTXEvent & p_event )
				{
					_openGLWidget->makeCurrent();
					if ( p_event.name == Event::Global::MOLECULE_CREATED )
					{
						const Event::VTXEventPtr<Model::Molecule> & castedEvent = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
						castedEvent.ptr->init( getOpenGLWidget().gl() );
					}
					else if ( p_event.name == Event::Global::MESH_CREATED )
					{
						const Event::VTXEventPtr<Model::MeshTriangle> & castedEvent = dynamic_cast<const Event::VTXEventPtr<Model::MeshTriangle> &>( p_event );
						castedEvent.ptr->init( getOpenGLWidget().gl() );
					}
					_openGLWidget->doneCurrent();
				}

				void RenderWidget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );

					setFocusPolicy( Qt::StrongFocus );
					setWidget( _openGLWidget );
				}

				void RenderWidget::_setupSlots() {}

				void RenderWidget::localize()
				{
					setWindowTitle( "Render" );
					// setWindowTitle( QCoreApplication::translate( "RenderWidget", "Render", nullptr ) );
				}

				void RenderWidget::mouseMoveEvent( QMouseEvent * p_event ) { Event::EventManager::get().fireEventMouse( p_event ); }

				void RenderWidget::mousePressEvent( QMouseEvent * p_event ) { Event::EventManager::get().fireEventMouse( p_event ); }

				void RenderWidget::mouseReleaseEvent( QMouseEvent * p_event ) { Event::EventManager::get().fireEventMouse( p_event ); }

				void RenderWidget::keyPressEvent( QKeyEvent * p_event ) { Event::EventManager::get().fireEventKeyboard( p_event ); }

				void RenderWidget::keyReleaseEvent( QKeyEvent * p_event ) { Event::EventManager::get().fireEventKeyboard( p_event ); }

				void RenderWidget::wheelEvent( QWheelEvent * p_event ) { Event::EventManager::get().fireEventWheel( p_event ); }

			} // namespace Render
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
