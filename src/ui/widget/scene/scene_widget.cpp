#include "scene_widget.hpp"
#include "action/action_manager.hpp"
//#include "action/atom.hpp"
#include "action/chain.hpp"
#include "action/molecule.hpp"
#include "action/residue.hpp"
#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/residue.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "scene_tree_widget_item.hpp"
#include "selection/selection_manager.hpp"
#include "tool/logger.hpp"
#include "ui/widget_factory.hpp"
#include "view/ui/widget/base_scene_item.hpp"
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

				void			  SceneWidget::addItem( QTreeWidgetItem * p_item ) { _treeWidget->addTopLevelItem( p_item ); }
				void			  SceneWidget::addItem( QTreeWidgetItem * p_item, QTreeWidgetItem * p_parent ) { p_parent->addChild( p_item ); }
				QTreeWidgetItem * SceneWidget::takeItem( QTreeWidgetItem * p_item )
				{
					_treeWidget->removeItemWidget( p_item, 0 );
					return p_item;
				}
				void SceneWidget::deleteItem( QTreeWidgetItem * const p_item )
				{
					_treeWidget->removeItemWidget( p_item, 0 );
					delete p_item;
				}

				void SceneWidget::receiveEvent( const Event::VTXEvent & p_event )
				{
					if ( p_event.name == Event::Global::MOLECULE_ADDED )
					{
						const Event::VTXEventPtr<Model::Molecule> & castedEvent = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );

						// Set no parent to not trigger ItemChange event during init
						View::UI::Widget::MoleculeSceneView * const item
							= WidgetFactory::get().GetSceneItem<View::UI::Widget::MoleculeSceneView, Model::Molecule>( castedEvent.ptr, nullptr, "MoleculeStructure" );

						MVC::MvcManager::get().addViewOnModel( castedEvent.ptr, ID::View::UI_MOLECULE_STRUCTURE, item );

						// Add Item to tree hierarchy
						addItem( item );
					}
					else if ( p_event.name == Event::Global::MOLECULE_REMOVED )
					{
						const Event::VTXEventPtr<Model::Molecule> & castedEvent = dynamic_cast<const Event::VTXEventPtr<Model::Molecule> &>( p_event );

						View::UI::Widget::MoleculeSceneView * const moleculeWidget
							= dynamic_cast<View::UI::Widget::MoleculeSceneView *>( MVC::MvcManager::get().removeViewOnModel( castedEvent.ptr, ID::View::UI_MOLECULE_STRUCTURE ) );

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

				void SceneWidget::_setupSlots()
				{
					connect( _treeWidget, &QTreeWidget::itemSelectionChanged, this, &SceneWidget::_onSelectionChange );
					connect( _treeWidget, &QTreeWidget::itemChanged, this, &SceneWidget::_onItemChange );
					connect( _treeWidget, &QTreeWidget::itemClicked, this, &SceneWidget::_onItemClicked );
				}

				void SceneWidget::_onItemChange( QTreeWidgetItem * p_item, int p_column )
				{
					if ( p_column == 0 )
					{
						const Model::Model_ID id		   = _getModelID( *p_item );
						const bool			  modelEnabled = p_item->checkState( 0 ) == Qt::CheckState::Checked ? true : false;

						_sendEnableStateChangeAction( id, modelEnabled );
					}
				}
				void SceneWidget::_onSelectionChange()
				{
					/*
				VTX_INFO( "_onSelectionChange" );
				VTX::Selection::SelectionManager & selectionManager = Selection::SelectionManager::get();
				selectionManager.clear();

				const QList<QTreeWidgetItem *> selectedObjects = _treeWidget->selectedItems();
				for ( auto iterator = selectedObjects.begin(); iterator != selectedObjects.end(); iterator++ )
				{
					QTreeWidgetItem *				 selectedObject = ( *iterator );
					VTX::Selection::BaseSelectable * item			= _getSelectableFromTreeWidgetItem( selectedObject );

					if ( item != nullptr )
						selectionManager.select( item );
				}
				*/
				}

				void SceneWidget::_onItemClicked( QTreeWidgetItem * p_item, int p_column )
				{
					const Model::Model_ID & id = _getModelID( *p_item );
				}

				void SceneWidget::localize()
				{
					this->setWindowTitle( "Scene" );
					// this->setWindowTitle( QCoreApplication::translate( "SceneWidget", "Scene", nullptr ) );
				}

				void SceneWidget::_sendEnableStateChangeAction( const Model::Model_ID & p_modelID, const bool modelEnabled ) const
				{
					ID::VTX_ID modelTypeId = MVC::MvcManager::get().getModelTypeID( p_modelID );

					if ( modelTypeId == ID::Model::MODEL_MOLECULE )
					{
						const Action::Visible::ChangeVisibility::VISIBILITY_MODE visibilityMode
							= modelEnabled ? Action::Visible::ChangeVisibility::VISIBILITY_MODE::SHOW : Action::Visible::ChangeVisibility::VISIBILITY_MODE::HIDE;

						Model::Molecule & model = MVC::MvcManager::get().getModel<Model::Molecule>( p_modelID );
						VTX_ACTION( new Action::Molecule::ChangeVisibility( model, visibilityMode ) );
					}
					else if ( modelTypeId == ID::Model::MODEL_CHAIN )
					{
						const Action::Visible::ChangeVisibility::VISIBILITY_MODE visibilityMode
							= modelEnabled ? Action::Visible::ChangeVisibility::VISIBILITY_MODE::SHOW : Action::Visible::ChangeVisibility::VISIBILITY_MODE::HIDE;

						Model::Chain & model = MVC::MvcManager::get().getModel<Model::Chain>( p_modelID );
						VTX_ACTION( new Action::Chain::ChangeVisibility( model, visibilityMode ) );
					}
					else if ( modelTypeId == ID::Model::MODEL_RESIDUE )
					{
						const Action::Visible::ChangeVisibility::VISIBILITY_MODE visibilityMode
							= modelEnabled ? Action::Visible::ChangeVisibility::VISIBILITY_MODE::SHOW : Action::Visible::ChangeVisibility::VISIBILITY_MODE::HIDE;

						Model::Residue & model = MVC::MvcManager::get().getModel<Model::Residue>( p_modelID );
						VTX_ACTION( new Action::Residue::ChangeVisibility( model, visibilityMode ) );
					}
				}
			} // namespace Scene
		}	  // namespace Widget
	}		  // namespace UI

} // namespace VTX
