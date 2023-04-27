#include "ui/old_ui/ui/widget/representation/instantiated_representation_list_widget.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/ui_action/self_referenced_action.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/util/ui.hpp"
#include <QRadioButton>
#include <QVBoxLayout>
#include <QVariant>
#include <app/old_app/generic/base_representable.hpp>
#include <app/model/atom.hpp>
#include <app/model/category.hpp>
#include <app/model/chain.hpp>
#include <app/model/molecule.hpp>
#include <app/model/residue.hpp>
#include <app/mvc.hpp>
#include <app/old_app/representation/representation_manager.hpp>
#include <app/old_app/selection/selection_manager.hpp>

namespace VTX::UI::Widget::Representation
{
	InstantiatedRepresentationListWidget::ModelFieldLine::ModelFieldLine(
		InstantiatedRepresentationListWidget * const p_owner ) :
		BaseModelFieldLine( p_owner ),
		_owner( p_owner )
	{
		_removeButton = new QPushButton( _owner );
		_removeButton->setIcon( Style::IconConst::get().DELETE_LINE_ICON );
		_removeButton->setFlat( true );
		connect( _removeButton, &QPushButton::clicked, this, &ModelFieldLine::_callRemoveAction );

		_modelButton = new QPushButton( _owner );
		_modelButton->setFlat( true );
		connect( _modelButton, &QPushButton::clicked, this, &ModelFieldLine::_onModelButtonClicked );
	}
	InstantiatedRepresentationListWidget::ModelFieldLine::~ModelFieldLine()
	{
		if ( _removeButton != nullptr )
			delete _removeButton;
		_removeButton = nullptr;

		if ( _modelButton != nullptr )
			delete _modelButton;
		_modelButton = nullptr;
	}

	void InstantiatedRepresentationListWidget::ModelFieldLine::setModel( App::Core::Model::BaseModel * const p_model )
	{
		_instantiatedRepresentation = static_cast<Model::Representation::InstantiatedRepresentation *>( p_model );

		const App::Core::Model::BaseModel * const target = _instantiatedRepresentation->getTarget()->getLinkedModel();

		const QString representationName
			= QString::fromStdString( _instantiatedRepresentation->getLinkedRepresentation()->getName() );

		std::string modelNameStr = "";
		if ( target->getTypeId() == ID::Model::MODEL_MOLECULE )
		{
			modelNameStr = target->getDefaultName();
		}
		else if ( target->getTypeId() == ID::Model::MODEL_CHAIN )
		{
			const Model::Chain * const	  chain	   = static_cast<const Model::Chain *>( target );
			const Model::Category * const category = chain->getMoleculePtr()->getCategoryFromChain( *chain );
			modelNameStr						   = category->getName() + "/" + chain->getDefaultName();
		}
		else if ( target->getTypeId() == ID::Model::MODEL_RESIDUE )
		{
			const Model::Residue * const  residue  = static_cast<const Model::Residue *>( target );
			const Model::Chain * const	  chain	   = residue->getChainPtr();
			const Model::Category * const category = chain->getMoleculePtr()->getCategoryFromChain( *chain );
			modelNameStr = category->getName() + "/" + chain->getDefaultName() + "/" + residue->getDefaultName();
		}

		const QString modelName = QString::fromStdString( modelNameStr );

		_modelButton->setText( representationName + " (" + modelName + ")" );
	}

	void InstantiatedRepresentationListWidget::ModelFieldLine::_onModelButtonClicked()
	{
		App::Core::Model::BaseModel * const targetModel = _instantiatedRepresentation->getTarget()->getLinkedModel();
		const ID::VTX_ID &		 modelTypeID = targetModel->getTypeId();

		if ( modelTypeID == VTX::ID::Model::MODEL_MOLECULE )
		{
			Model::Molecule * const molecule = static_cast<Model::Molecule *>( targetModel );
			VTX::Selection::SelectionManager::get().getSelectionModel().selectMolecule( *molecule );
		}
		else if ( modelTypeID == VTX::ID::Model::MODEL_CHAIN )
		{
			Model::Chain * const chain = static_cast<Model::Chain *>( targetModel );
			VTX::Selection::SelectionManager::get().getSelectionModel().selectChain( *chain );
		}
		else if ( modelTypeID == VTX::ID::Model::MODEL_RESIDUE )
		{
			Model::Residue * const residue = static_cast<Model::Residue *>( targetModel );
			VTX::Selection::SelectionManager::get().getSelectionModel().selectResidue( *residue );
		}
		else if ( modelTypeID == VTX::ID::Model::MODEL_ATOM )
		{
			Model::Atom * const atom = static_cast<Model::Atom *>( targetModel );
			VTX::Selection::SelectionManager::get().getSelectionModel().selectAtom( *atom );
		}
	}

	void InstantiatedRepresentationListWidget::ModelFieldLine::_callRemoveAction()
	{
		App::Core::Model::BaseModel * targetModel		   = _instantiatedRepresentation->getTarget()->getLinkedModel();
		const ID::VTX_ID & representableTypeID = targetModel->getTypeId();

		if ( representableTypeID == ID::Model::MODEL_MOLECULE )
		{
			Model::Molecule * const molecule = static_cast<Model::Molecule *>( targetModel );
			VTX::Representation::RepresentationManager::get().removeInstantiatedRepresentation( *molecule );
		}
		else if ( representableTypeID == ID::Model::MODEL_CHAIN )
		{
			Model::Chain * const chain = static_cast<Model::Chain *>( targetModel );
			VTX::Representation::RepresentationManager::get().removeInstantiatedRepresentation( *chain );
		}
		else if ( representableTypeID == ID::Model::MODEL_CHAIN )
		{
			Model::Residue * const residue = static_cast<Model::Residue *>( targetModel );
			VTX::Representation::RepresentationManager::get().removeInstantiatedRepresentation( *residue );
		}

		_owner->removeModel( _instantiatedRepresentation );
	}

	InstantiatedRepresentationListWidget::InstantiatedRepresentationListWidget( QWidget * p_parent ) :
		CustomWidget::BaseModelListWidget( p_parent )
	{
	}

	void InstantiatedRepresentationListWidget::_initColumns()
	{
		_initColumn( int( GRID_LAYOUT_COLUMN::MODEL ), "", 1000 );
		_initColumn( int( GRID_LAYOUT_COLUMN::REMOVE_BUTTON ), "", 0 );
	}

	CustomWidget::BaseModelFieldLine * const InstantiatedRepresentationListWidget::_instantiateLine()
	{
		ModelFieldLine * const newLine = new ModelFieldLine( this );
		return newLine;
	}
	void InstantiatedRepresentationListWidget::_addLineInLayout( CustomWidget::BaseModelFieldLine * const p_line,
																 const int								  p_row )
	{
		const ModelFieldLine * const castedLine = dynamic_cast<ModelFieldLine * const>( p_line );

		_addWidgetInColumn( castedLine->getModelButton(), p_row, int( GRID_LAYOUT_COLUMN::MODEL ) );
		_addWidgetInColumn( castedLine->getRemoveButton(), p_row, int( GRID_LAYOUT_COLUMN::REMOVE_BUTTON ) );
	}
} // namespace VTX::UI::Widget::Representation
