#include "scene_widget.hpp"
#include "model/molecule.hpp"
#include "mvc/mvc_manager.hpp"
#include "ui/widget_factory.hpp"
#include "view/ui/widget/molecule_scene_view.hpp"

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
							= WidgetFactory::get().getViewWidget<View::UI::Widget::MoleculeSceneView, Model::Molecule, QTreeWidget>(
								castedEvent.ptr, nullptr, "MoleculeStructure" );

						MVC::MvcManager::get().addViewOnModel( castedEvent.ptr, ID::View::UI_MOLECULE_STRUCTURE, moleculeWidget );

						// Add Item to tree hierarchy
						_layout->addWidget( moleculeWidget );
					}
					else if ( p_event.name == Event::Global::MOLECULE_REMOVED )
					{
						const Event::VTXEventPtr<Model::Molecule> & castedEvent = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
						const Model::Molecule * const				molecule	= castedEvent.ptr;

						View::UI::Widget::MoleculeSceneView * const moleculeWidget
							= MVC::MvcManager::get().removeViewOnModel<Model::Molecule, View::UI::Widget::MoleculeSceneView>( molecule, ID::View::UI_MOLECULE_STRUCTURE );

						_layout->removeWidget( moleculeWidget );
						delete moleculeWidget;
					}
				}

				void SceneWidget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );

					_widget = new QWidget( this );
					_layout = new QVBoxLayout( _widget );

					setWidget( _widget );
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
