#include "sequence_widget.hpp"
#include "model/molecule.hpp"
#include "molecule_sequence_widget.hpp"
#include "mvc/mvc_manager.hpp"
#include "view/base_view.hpp"
#include "view/ui/widget/molecule_sequence_view.hpp"
#include "vtx_app.hpp"
#include <QLayout>
#include <QScrollBar>
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Sequence
			{
				SequenceWidget::SequenceWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
				{
					_registerEvent( Event::Global::MOLECULE_ADDED );
					_registerEvent( Event::Global::MOLECULE_REMOVED );
				}

				void SequenceWidget::receiveEvent( const Event::VTXEvent & p_event )
				{
					if ( p_event.name == Event::Global::MOLECULE_ADDED )
					{
						const Event::VTXEventPtr<Model::Molecule> &	   castedEvent			= dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
						View::UI::Widget::MoleculeSequenceView * const moleculeSequenceView = new View::UI::Widget::MoleculeSequenceView( castedEvent.ptr, nullptr );

						MVC::MvcManager::get().addViewOnModel( castedEvent.ptr, ID::View::UI_MOLECULE_SEQUENCE, moleculeSequenceView );
						MoleculeSequenceWidget * const widget = moleculeSequenceView->getWidget();

						this->_scrollAreaLayout->addWidget( widget );
					}
					else if ( p_event.name == Event::Global::MOLECULE_REMOVED )
					{
						const Event::VTXEventPtr<Model::Molecule> &	   castedEvent = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
						View::BaseView * const						   baseView	   = MVC::MvcManager::get().removeViewOnModel( castedEvent.ptr, ID::View::UI_MOLECULE_SEQUENCE );
						View::UI::Widget::MoleculeSequenceView * const moleculeSequenceView = dynamic_cast<View::UI::Widget::MoleculeSequenceView * const>( baseView );

						delete moleculeSequenceView;
					}
				}

				void SequenceWidget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );

					_scrollArea = new QScrollArea( this );

					QWidget * const scrollAreaContent = new QWidget();
					scrollAreaContent->setContentsMargins( 0, 0, 0, 0 );
					_scrollAreaLayout = new QVBoxLayout( scrollAreaContent );
					_scrollAreaLayout->setSizeConstraint( QLayout::SizeConstraint::SetMinAndMaxSize );

					_scrollArea->setWidget( scrollAreaContent );

					setWidget( _scrollArea );
				}

				void SequenceWidget::_setupSlots() {}

				void SequenceWidget::localize()
				{
					this->setWindowTitle( "Sequence" );
					// this->setWindowTitle( QCoreApplication::translate( "SceneWidget", "Scene", nullptr ) );
				}
			} // namespace Sequence
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
