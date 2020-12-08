#include "scene_widget.hpp"
#include "model/molecule.hpp"
#include "mvc/mvc_manager.hpp"
#include "ui/widget_factory.hpp"
#include "view/ui/widget/molecule_scene_view.hpp"
#include <QScrollArea>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Scene
			{
				SceneWidget::SceneWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
				{
					_registerEvent( Event::Global::MOLECULE_ADDED );
					_registerEvent( Event::Global::MOLECULE_REMOVED );
				}

				void SceneWidget::receiveEvent( const Event::VTXEvent & p_event )
				{
					if ( p_event.name == Event::Global::MOLECULE_ADDED )
					{
						const Event::VTXEventPtr<Model::Molecule> & castedEvent = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );

						// Set no parent to not trigger ItemChange event during init
						View::UI::Widget::MoleculeSceneView * const moleculeWidget
							= WidgetFactory::get().instanciateViewWidget<View::UI::Widget::MoleculeSceneView, Model::Molecule, QTreeWidget>(
								castedEvent.ptr, ID::View::UI_MOLECULE_STRUCTURE, _scrollAreaContent, "MoleculeStructure" );

						// Add Item to tree hierarchy
						_layout->insertWidget( _layout->count() - 1, moleculeWidget, 1 );
					}
					else if ( p_event.name == Event::Global::MOLECULE_REMOVED )
					{
						const Event::VTXEventPtr<Model::Molecule> & castedEvent = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
						const Model::Molecule * const				molecule	= castedEvent.ptr;

						View::UI::Widget::MoleculeSceneView * const moleculeWidget
							= MVC::MvcManager::get().getView<View::UI::Widget::MoleculeSceneView>( molecule, ID::View::UI_MOLECULE_STRUCTURE );

						_layout->removeWidget( moleculeWidget );

						MVC::MvcManager::get().deleteView<View::UI::Widget::MoleculeSceneView>( molecule, ID::View::UI_MOLECULE_STRUCTURE );
					}
				}

				void SceneWidget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );

					_scrollAreaContent = new QWidget( this );
					_scrollAreaContent->setSizePolicy( QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum );

					_layout = new QVBoxLayout( _scrollAreaContent );
					_layout->setSizeConstraint( QLayout::SizeConstraint::SetMinAndMaxSize );
					_layout->setSpacing( 2 );
					_layout->addStretch( 1000 );
					_layout->setContentsMargins( 0, 0, 0, 0 );

					QScrollArea * const scrollArea = new QScrollArea( this );
					scrollArea->setFrameShape( QFrame::Shape::NoFrame );
					scrollArea->setWidget( _scrollAreaContent );
					scrollArea->setWidgetResizable( true );
					scrollArea->setSizeAdjustPolicy( QAbstractScrollArea::SizeAdjustPolicy::AdjustIgnored );

					setWidget( scrollArea );
				}

				void SceneWidget::_setupSlots() {}

				void SceneWidget::localize()
				{
					this->setWindowTitle( "Scene" );
					// this->setWindowTitle( QCoreApplication::translate( "SceneWidget", "Scene", nullptr ) );
				}

			} // namespace Scene
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
