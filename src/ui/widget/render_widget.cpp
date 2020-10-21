#include "render_widget.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			RenderWidget::RenderWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
			{
				_registerEvent( Event::Global::MOLECULE_ADDED );
				_registerEvent( Event::Global::MESH_ADDED );
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
				if ( p_event.name == Event::Global::MOLECULE_ADDED )
				{
					const Event::VTXEventPtr<Model::Molecule> & castedEvent = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
					castedEvent.ptr->init( getOpenGLWidget().gl() );
				}
				else if ( p_event.name == Event::Global::MESH_ADDED )
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

				_verticalLayoutWidget = new QWidget();
				_verticalLayoutWidget->setObjectName( QString::fromUtf8( "verticalLayoutWidget" ) );
				_verticalLayout = new QVBoxLayout( _verticalLayoutWidget );
				_verticalLayout->setObjectName( QString::fromUtf8( "verticalLayout" ) );
				setWidget( _verticalLayoutWidget );
				_verticalLayout->addWidget( _openGLWidget );
			}

			void RenderWidget::_setupSlots() {}

			void RenderWidget::localize()
			{
				setWindowTitle( "Render" );
				// setWindowTitle( QCoreApplication::translate( "RenderWidget", "Render", nullptr ) );
			}

			void RenderWidget::mouseMoveEvent( QMouseEvent * p_event ) { VTXApp::get().getEventManager().fireEventMouse( p_event ); }

			void RenderWidget::mousePressEvent( QMouseEvent * p_event ) { VTXApp::get().getEventManager().fireEventMouse( p_event ); }

			void RenderWidget::mouseReleaseEvent( QMouseEvent * p_event ) { VTXApp::get().getEventManager().fireEventMouse( p_event ); }

			void RenderWidget::keyPressEvent( QKeyEvent * p_event ) { VTXApp::get().getEventManager().fireEventKeyboard( p_event ); }

			void RenderWidget::keyReleaseEvent( QKeyEvent * p_event ) { VTXApp::get().getEventManager().fireEventKeyboard( p_event ); }

			void RenderWidget::wheelEvent( QWheelEvent * p_event ) { VTXApp::get().getEventManager().fireEventWheel( p_event ); }

		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
