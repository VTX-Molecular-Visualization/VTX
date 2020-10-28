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

				void MoleculeSceneView::_setupUi( const QString & p_name )
				{
					BaseSceneItem<Model::Molecule>::_setupUi( p_name );
					setData( 0, Qt::UserRole, QVariant::fromValue<VTX::Model::Model_ID>( _model->getId() ) );
					_refreshItem( this, *_model );

					// Chains.
					for ( Model::Chain * const chain : _model->getChains() )
					{
						QTreeWidgetItem * const chainView = new QTreeWidgetItem( this );
						chainView->setData( 0, Qt::UserRole, QVariant::fromValue( chain->getId() ) );
						_refreshItem( chainView, *chain );

						addChild( chainView );

						// Residues.
						for ( uint r = 0; r < chain->getResidueCount(); ++r )
						{
							Model::Residue &		residue		= _model->getResidue( chain->getIndexFirstResidue() + r );
							QTreeWidgetItem * const residueView = new QTreeWidgetItem( chainView );
							residueView->setData( 0, Qt::UserRole, QVariant::fromValue( residue.getId() ) );
							_refreshItem( residueView, residue );
							chainView->addChild( residueView );
						}
					}
				}

				void MoleculeSceneView::_setupSlots() {}

				void MoleculeSceneView::localize() {}

				void MoleculeSceneView::refreshItem( QTreeWidgetItem * const p_itemWidget )
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

				void MoleculeSceneView::_refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Molecule & p_model ) const
				{
					p_itemWidget->setText( 0, QString::fromStdString( p_model.getDefaultName() ) );
					p_itemWidget->setCheckState( 0, _getCheckState( p_model.isVisible() ) );
					p_itemWidget->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( p_model.getTypeId() ) );
				}
				void MoleculeSceneView::_refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Chain & p_model ) const
				{
					p_itemWidget->setText( 0, QString::fromStdString( p_model.getDefaultName() ) );
					p_itemWidget->setCheckState( 0, _getCheckState( p_model.isVisible() ) );
					p_itemWidget->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( p_model.getTypeId() ) );
				}
				void MoleculeSceneView::_refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Residue & p_model ) const
				{
					p_itemWidget->setText( 0, QString::fromStdString( p_model.getDefaultName() ) );
					p_itemWidget->setCheckState( 0, _getCheckState( p_model.isVisible() ) );
					p_itemWidget->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( p_model.getTypeId() ) );
				}
				void MoleculeSceneView::_refreshItem( QTreeWidgetItem * const p_itemWidget, const Model::Atom & p_model ) const
				{
					p_itemWidget->setText( 0, QString::fromStdString( p_model.getDefaultName() ) );
					p_itemWidget->setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( p_model.getTypeId() ) );
				}

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
