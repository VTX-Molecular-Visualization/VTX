#include "molecule_scene_view.hpp"
#include "action/action_manager.hpp"
#include "action/atom.hpp"
#include "action/chain.hpp"
#include "action/molecule.hpp"
#include "action/residue.hpp"
#include "action/selection.hpp"
#include "id.hpp"
#include "mvc/mvc_manager.hpp"
#include "style.hpp"
#include "ui/widget_factory.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				void MoleculeSceneView::notify( const Event::VTXEvent * const p_event )
				{
					if ( p_event->name == Event::Model::MOLECULE_VISIBILITY )
					{
						_refreshItem( topLevelItem( 0 ), *_model );
					}
					else if ( p_event->name == Event::Model::CHAIN_VISIBILITY )
					{
						const Event::VTXEventValue<uint> * const castedEventData = dynamic_cast<const Event::VTXEventValue<uint> *>( p_event );
						const uint								 index			 = castedEventData->value;
						const Model::Chain &					 chain			 = _model->getChain( index );
						_refreshItem( topLevelItem( 0 )->child( index ), chain );
					}
					else if ( p_event->name == Event::Model::RESIDUE_VISIBILITY )

					{
						const Event::VTXEventValue<uint> * const castedEventData = dynamic_cast<const Event::VTXEventValue<uint> *>( p_event );
						const uint								 index			 = castedEventData->value;
						const Model::Residue &					 residue		 = _model->getResidue( index );
						const Model::Chain &					 chain			 = *residue.getChainPtr();
						_refreshItem( topLevelItem( 0 )->child( chain.getIndex() )->child( residue.getIndex() - chain.getIndexFirstResidue() ), residue );
					}
				}

				void MoleculeSceneView::_setupUi( const QString & p_name )
				{
					setObjectName( QString::fromUtf8( "sceneTree" ) );
					setColumnCount( 1 );
					setHeaderHidden( true );

					QTreeWidgetItem * const moleculeView = new QTreeWidgetItem();

					moleculeView->setData( 0, Qt::UserRole, QVariant::fromValue<VTX::Model::ID>( _model->getId() ) );
					moleculeView->setText( 0, QString::fromStdString( _model->getDefaultName() ) );
					moleculeView->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( _model->getTypeId() ) );
					_refreshItem( moleculeView, *_model );

					// Chains.
					for ( const Model::Chain * const chain : _model->getChains() )
					{
						QTreeWidgetItem * const chainView = new QTreeWidgetItem();
						chainView->setData( 0, Qt::UserRole, QVariant::fromValue( chain->getId() ) );
						chainView->setText( 0, QString::fromStdString( chain->getDefaultName() ) );
						chainView->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( chain->getTypeId() ) );
						_refreshItem( chainView, *chain );

						// Residues.
						for ( uint r = 0; r < chain->getResidueCount(); ++r )
						{
							const Model::Residue &	residue		= _model->getResidue( chain->getIndexFirstResidue() + r );
							QTreeWidgetItem * const residueView = new QTreeWidgetItem();
							residueView->setData( 0, Qt::UserRole, QVariant::fromValue( residue.getId() ) );
							residueView->setText( 0, QString::fromStdString( residue.getSymbolStr() + " " + std::to_string( residue.getIndex() ) ) );
							residueView->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( residue.getTypeId() ) );
							_refreshItem( residueView, residue );

							// Atom.
							for ( uint a = 0; a < residue.getAtomCount(); ++a )
							{
								const Model::Atom &		atom	 = _model->getAtom( residue.getIndexFirstAtom() + a );
								QTreeWidgetItem * const atomView = new QTreeWidgetItem();
								atomView->setData( 0, Qt::UserRole, QVariant::fromValue( atom.getId() ) );
								atomView->setText( 0, QString::fromStdString( atom.getSymbolStr() + " " + std::to_string( atom.getIndex() ) ) );
								atomView->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( atom.getTypeId() ) );

								residueView->addChild( atomView );
							}

							chainView->addChild( residueView );
						}

						moleculeView->addChild( chainView );
					}

					addTopLevelItem( moleculeView );
				}

				void MoleculeSceneView::_setupSlots()
				{
					connect( this, &QTreeWidget::itemChanged, this, &MoleculeSceneView::_onItemChanged );
					connect( this, &QTreeWidget::itemClicked, this, &MoleculeSceneView::_onItemClicked );
					connect( this, &QTreeWidget::itemExpanded, this, &MoleculeSceneView::_onItemExpanded );
					connect( this, &QTreeWidget::itemCollapsed, this, &MoleculeSceneView::_onItemCollapsed );
				}

				void MoleculeSceneView::localize() {}

				void MoleculeSceneView::_onItemChanged( QTreeWidgetItem * p_item, int p_column )
				{
					if ( p_column == 0 )
					{
						const Model::ID id			 = _getModelID( *p_item );
						const bool		modelEnabled = p_item->checkState( 0 ) == Qt::CheckState::Checked ? true : false;

						_sendEnableStateChangeAction( id, modelEnabled );
					}
				}

				void MoleculeSceneView::_onItemClicked( QTreeWidgetItem * p_item, int p_column )
				{
					const Model::ID &  modelId		  = _getModelID( *p_item );
					ID::VTX_ID		   modelTypeId	  = MVC::MvcManager::get().getModelTypeID( modelId );
					Model::Selection & selectionModel = VTX::Selection::SelectionManager::get().getSelectionModel();

					if ( modelTypeId == ID::Model::MODEL_MOLECULE )
					{
						const Model::Molecule & model = MVC::MvcManager::get().getModel<Model::Molecule>( modelId );
						VTX_ACTION( new Action::Selection::SelectMolecule( selectionModel, model ) );
					}
					else if ( modelTypeId == ID::Model::MODEL_CHAIN )
					{
						const Model::Chain & model = MVC::MvcManager::get().getModel<Model::Chain>( modelId );
						VTX_ACTION( new Action::Selection::SelectChain( selectionModel, model ) );
					}
					else if ( modelTypeId == ID::Model::MODEL_RESIDUE )
					{
						const Model::Residue & model = MVC::MvcManager::get().getModel<Model::Residue>( modelId );
						VTX_ACTION( new Action::Selection::SelectResidue( selectionModel, model ) );
					}
					else if ( modelTypeId == ID::Model::MODEL_ATOM )
					{
						const Model::Atom & model = MVC::MvcManager::get().getModel<Model::Atom>( modelId );
						VTX_ACTION( new Action::Selection::SelectAtom( selectionModel, model ) );
					}
				}

				void MoleculeSceneView::_onItemExpanded( QTreeWidgetItem * p_item ) {}

				void MoleculeSceneView::_onItemCollapsed( QTreeWidgetItem * p_item ) {}

				void MoleculeSceneView::_refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Molecule & p_model ) const
				{
					const Qt::CheckState newCheckState = _getCheckState( p_model.isVisible() );
					if ( p_itemWidget->checkState( 0 ) != newCheckState )
						p_itemWidget->setCheckState( 0, newCheckState );
				}

				void MoleculeSceneView::_refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Chain & p_model ) const
				{
					const Qt::CheckState newCheckState = _getCheckState( p_model.isVisible() );
					if ( p_itemWidget->checkState( 0 ) != newCheckState )
						p_itemWidget->setCheckState( 0, newCheckState );
				}

				void MoleculeSceneView::_refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Residue & p_model ) const
				{
					const Qt::CheckState newCheckState = _getCheckState( p_model.isVisible() );
					if ( p_itemWidget->checkState( 0 ) != newCheckState )
						p_itemWidget->setCheckState( 0, newCheckState );
				}

				void MoleculeSceneView::_sendEnableStateChangeAction( const Model::ID & p_modelID, const bool modelEnabled ) const
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

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
