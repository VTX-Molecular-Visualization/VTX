#include "scene_widget.hpp"
#include "base_widget.hpp"
#include "model/molecule.hpp"
#include "object3d/scene.hpp"
#include "ui/widget_factory.hpp"
#include "view/ui/widget/molecule_scene_view.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			SceneWidget::SceneWidget( QWidget * p_parent ) : BaseManualWidget( p_parent )
			{
				_registerEvent( Event::Global::MOLECULE_ADDED );
				_registerEvent( Event::Global::MOLECULE_REMOVED );
			}

			void			  SceneWidget::addItem( QTreeWidgetItem * p_item ) { _treeWidget->addTopLevelItem( p_item ); }
			void			  SceneWidget::addItem( QTreeWidgetItem * p_item, QTreeWidgetItem * p_parent ) { p_parent->addChild( p_item ); }
			QTreeWidgetItem * SceneWidget::takeItem( QTreeWidgetItem * p_item )
			{
				_treeWidget->removeItemWidget( p_item, 0 );
				return p_item;
			}
			void SceneWidget::deleteItem( QTreeWidgetItem * p_item )
			{
				_treeWidget->removeItemWidget( p_item, 0 );
				delete p_item;
			}

			void SceneWidget ::receiveEvent( const Event::VTXEvent & p_event )
			{
				if ( p_event.name == Event::Global::MOLECULE_ADDED )
				{
					const Event::VTXEventPtr<Model::Molecule> & castedEvent = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );

					View::UI::Widget::MoleculeSceneView * item = WidgetFactory::get().GetSceneItem<View::UI::Widget::MoleculeSceneView, Model::Molecule>(
						castedEvent.ptr, _treeWidget->invisibleRootItem(), "MoleculeStructure" );

					castedEvent.ptr->addItem( ID::View::UI_MOLECULE_STRUCTURE, item );
					addItem( item );
				}
				else if ( p_event.name == Event::Global::MOLECULE_REMOVED )
				{
					const Event::VTXEventPtr<Model::Molecule> & castedEvent	   = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );
					View::UI::Widget::MoleculeSceneView *		moleculeWidget = castedEvent.ptr->getItem<View::UI::Widget::MoleculeSceneView>( ID::View::UI_MOLECULE_STRUCTURE );

					deleteItem( moleculeWidget );
				}
			}

			void SceneWidget::_setupUi( const QString & p_name )
			{
				BaseManualWidget::_setupUi( p_name );

				_treeWidget = new QTreeWidget( this );
				_treeWidget->setObjectName( QString::fromUtf8( "sceneTree" ) );
				_treeWidget->setColumnCount( 1 );
				_treeWidget->setHeaderHidden( true );

				setWidget( _treeWidget );
			}

			void SceneWidget::_setupSlots() {}
			void SceneWidget::localize()
			{
				this->setWindowTitle( "Scene" );
				// this->setWindowTitle( QCoreApplication::translate( "SceneWidget", "Scene", nullptr ) );
			}
		} // namespace Widget
	}	  // namespace UI

} // namespace VTX
