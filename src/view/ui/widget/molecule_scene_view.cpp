#include "molecule_scene_view.hpp"
#include "id.hpp"
#include "molecule_subdata_scene_view.hpp"
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
				MoleculeSceneView::MoleculeSceneView( Model::Molecule * const p_model, QTreeWidgetItem * p_parent ) : BaseSceneItem<Model::Molecule>( p_model, p_parent ) {}

				void MoleculeSceneView::notify( const Event::VTX_EVENT_MODEL & p_event, const Event::VTXEventModelData * const p_eventData )
				{
					if ( p_event == Event::VTX_EVENT_MODEL::CHILD_DATA_CHANGE )
					{
						const Event::VTXEventModelDataTemplated<Model::Model_ID> * const castedEventData
							= dynamic_cast<const Event::VTXEventModelDataTemplated<Model::Model_ID> *>( p_eventData );

						const Model::Model_ID idChildModified = castedEventData->value;
						_refreshItem( idChildModified );
					}
				}

				void MoleculeSceneView::_setupUi( const QString & p_name )
				{
					BaseSceneItem<Model::Molecule>::_setupUi( p_name );
					setData( 0, Qt::UserRole, QVariant::fromValue<VTX::Model::Model_ID>( _model->getId() ) );
					setText( 0, QString::fromStdString( _model->getDefaultName() ) );
					setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( _model->getTypeId() ) );

					_refreshItem( this, *_model );

					// Chains.
					for ( Model::Chain * const chain : _model->getChains() )
					{
						QTreeWidgetItem * const chainView = new QTreeWidgetItem( this );
						chainView->setData( 0, Qt::UserRole, QVariant::fromValue( chain->getId() ) );
						chainView->setText( 0, QString::fromStdString( chain->getDefaultName() ) );
						chainView->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( chain->getTypeId() ) );
						_refreshItem( chainView, *chain );

						addChild( chainView );

						// Residues.
						for ( uint r = 0; r < chain->getResidueCount(); ++r )
						{
							Model::Residue &		residue		= _model->getResidue( chain->getIndexFirstResidue() + r );
							QTreeWidgetItem * const residueView = new QTreeWidgetItem( chainView );
							residueView->setData( 0, Qt::UserRole, QVariant::fromValue( residue.getId() ) );
							residueView->setText( 0, QString::fromStdString( residue.getDefaultName() ) );
							residueView->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( residue.getTypeId() ) );
							_refreshItem( residueView, residue );

							chainView->addChild( residueView );
						}
					}
				}

				void MoleculeSceneView::_setupSlots() {}
				void MoleculeSceneView::localize() {}

				void MoleculeSceneView::_refreshItem( QTreeWidgetItem * const p_itemWidget )
				{
					Model::Model_ID	   modelId = p_itemWidget->data( 0, Qt::UserRole ).value<VTX::Model::Model_ID>();
					const ID::VTX_ID & typeId  = MVC::MvcManager::get().getModelTypeID( modelId );

					if ( typeId == ID::Model::MODEL_MOLECULE )
						_refreshItem( p_itemWidget, MVC::MvcManager::get().getModel<Model::Molecule>( modelId ) );
					else if ( typeId == ID::Model::MODEL_CHAIN )
						_refreshItem( p_itemWidget, MVC::MvcManager::get().getModel<Model::Chain>( modelId ) );
					else if ( typeId == ID::Model::MODEL_RESIDUE )
						_refreshItem( p_itemWidget, MVC::MvcManager::get().getModel<Model::Residue>( modelId ) );
					else if ( typeId == ID::Model::MODEL_ATOM )
						_refreshItem( p_itemWidget, MVC::MvcManager::get().getModel<Model::Atom>( modelId ) );
				}
				void MoleculeSceneView::_refreshItem( const Model::Model_ID & p_id )
				{
					const ID::VTX_ID & typeId = MVC::MvcManager::get().getModelTypeID( p_id );

					if ( typeId == ID::Model::MODEL_MOLECULE )
					{
						_refreshItem( this );
					}
					else if ( typeId == ID::Model::MODEL_CHAIN )
					{
						const Model::Chain &	chain = MVC::MvcManager::get().getModel<Model::Chain>( p_id );
						QTreeWidgetItem * const item  = child( chain.getIndex() );

						_refreshItem( item, chain );
					}
					else if ( typeId == ID::Model::MODEL_RESIDUE )
					{
						const Model::Residue &	   residue = MVC::MvcManager::get().getModel<Model::Residue>( p_id );
						const Model::Chain * const chain   = residue.getChainPtr();
						QTreeWidgetItem * const	   item	   = child( chain->getIndex() )->child( residue.getIndexInOriginalChain() - 1 );

						_refreshItem( item, residue );
					}
					else if ( typeId == ID::Model::MODEL_ATOM )
					{
						// const Model::Atom &			 atom	 = MVC::MvcManager::get().getModel<Model::Atom>( p_id );
						// const Model::Residue * const residue = atom.getResiduePtr();
						// const Model::Chain * const	 chain	 = residue->getChainPtr();
						// QTreeWidgetItem * const		 item	 = child( chain->getIndex() )->child( residue->getIndexInOriginalChain() );

						//_refreshItem( item, atom );
					}
				}

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
				void MoleculeSceneView::_refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Atom & p_model ) const {}

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
