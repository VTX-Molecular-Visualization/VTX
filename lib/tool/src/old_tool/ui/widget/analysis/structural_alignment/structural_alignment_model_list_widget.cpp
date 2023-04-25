#include "tool/old_tool/ui/widget/analysis/structural_alignment/structural_alignment_model_list_widget.hpp"
#include <QRadioButton>
#include <QVBoxLayout>
#include <QVariant>
#include <app/mvc.hpp>
#include <ui/old_ui/style.hpp>
#include <ui/old_ui/ui/ui_action/self_referenced_action.hpp>
#include <ui/old_ui/ui/widget_factory.hpp>
#include <ui/old_ui/util/ui.hpp>

namespace VTX::UI::Widget::Analysis::StructuralAlignment
{
	StructuralAlignmentModelListWidget::ModelFieldLine::ModelFieldLine(
		StructuralAlignmentModelListWidget * const p_owner ) :
		BaseModelFieldLine( p_owner ),
		_owner( p_owner )
	{
		_tickButton = new QRadioButton( _owner );

		_removeButton = new QPushButton( _owner );
		_removeButton->setIcon( Style::IconConst::get().REMOVE_LINE_ICON );
		_removeButton->setFlat( true );

		connect( _removeButton, &QPushButton::clicked, this, &ModelFieldLine::_callRemoveAction );

		_modelField = WidgetFactory::get().instantiateWidget<CustomWidget::ModelFieldWidget>( _owner, "modelField" );

		for ( const ID::VTX_ID & typeID : _owner->getFilters() )
			_modelField->addTypeFilter( typeID );

		connect(
			_modelField, &CustomWidget::ModelFieldWidget::onModelChanged, this, &ModelFieldLine::_checkModelChange );

		_rmsdLabel = new QLabel( _owner );
		resetRMSD();
	}
	StructuralAlignmentModelListWidget::ModelFieldLine::~ModelFieldLine()
	{
		if ( _tickButton != nullptr )
			delete _tickButton;
		_tickButton = nullptr;

		if ( _removeButton != nullptr )
			delete _removeButton;
		_removeButton = nullptr;

		if ( _modelField != nullptr )
			delete _modelField;
		_modelField = nullptr;

		if ( _grabButton != nullptr )
			delete _grabButton;
		_grabButton = nullptr;

		if ( _rmsdLabel != nullptr )
			delete _rmsdLabel;
		_rmsdLabel = nullptr;
	}

	void StructuralAlignmentModelListWidget::ModelFieldLine::setModel( Model::BaseModel * const p_model )
	{
		_modelField->setModel( p_model );
		resetRMSD();
	}

	bool StructuralAlignmentModelListWidget::ModelFieldLine::isTicked() const { return _tickButton->isChecked(); }
	void StructuralAlignmentModelListWidget::ModelFieldLine::setTickState( const bool p_ticked ) const
	{
		_tickButton->setChecked( p_ticked );
	}
	void StructuralAlignmentModelListWidget::ModelFieldLine::setRMSD( const float p_rmsd, const size_t p_residueCount )
	{
		_rmsdLabel->setText( QString::fromStdString( std::to_string( p_rmsd ) + " (over "
													 + std::to_string( p_residueCount ) + " residues)" ) );
	}
	void StructuralAlignmentModelListWidget::ModelFieldLine::resetRMSD()
	{
		_rmsdLabel->setText( Style::ALIGNMENT_WINDOW_UNKNOWN_RMSD );
	}

	void StructuralAlignmentModelListWidget::ModelFieldLine::_callRemoveAction()
	{
		Model::BaseModel * const model = _modelField->getModel();
		_owner->removeModel( model );
	}
	void StructuralAlignmentModelListWidget::ModelFieldLine::_checkModelChange( Model::BaseModel * const p_model )
	{
		if ( _owner->getContainsOnlyUniqueModel() && _owner->hasModel( p_model ) )
			_owner->swapModels( _modelField->getModel(), p_model );
		else
			resetRMSD();
	}

	bool StructuralAlignmentModelListWidget::ModelFieldLine::blockSignals( const bool p_block )
	{
		_modelField->blockSignals( p_block );
		return CustomWidget::BaseModelFieldLine::blockSignals( p_block );
	};

	StructuralAlignmentModelListWidget::StructuralAlignmentModelListWidget( QWidget * p_parent ) :
		CustomWidget::BaseModelListWidget( p_parent )
	{
	}

	void StructuralAlignmentModelListWidget::_onLineWillBeRemoved( CustomWidget::BaseModelFieldLine * const p_line )
	{
		ModelFieldLine * castedLine = dynamic_cast<ModelFieldLine *>( p_line );
		if ( castedLine->isTicked() )
		{
			CustomWidget::BaseModelFieldLine * const lineToCheck = _findNextLine( p_line );

			ModelFieldLine * const castedLineToCheck = dynamic_cast<ModelFieldLine * const>( lineToCheck );
			if ( castedLineToCheck != nullptr )
				castedLineToCheck->setTickState( true );
		}
	}

	Model::BaseModel * const StructuralAlignmentModelListWidget::getTickedModel() const
	{
		for ( CustomWidget::BaseModelFieldLine * const line : _getLines() )
		{
			ModelFieldLine * const castedLine = dynamic_cast<ModelFieldLine * const>( line );
			if ( castedLine->isTicked() )
				return castedLine->getModel();
		}

		return nullptr;
	}
	std::vector<Model::BaseModel *> StructuralAlignmentModelListWidget::getNotTickedModels() const
	{
		std::vector<Model::BaseModel *> res = std::vector<Model::BaseModel *>();
		res.reserve( _getLines().size() );

		for ( CustomWidget::BaseModelFieldLine * const line : _getLines() )
		{
			ModelFieldLine * const castedLine = dynamic_cast<ModelFieldLine * const>( line );
			if ( !castedLine->isTicked() )
				res.emplace_back( castedLine->getModel() );
		}

		res.shrink_to_fit();

		return res;
	}

	void StructuralAlignmentModelListWidget::setRMSD( const Model::BaseModel * const p_model,
													  const float					 p_rmsd,
													  const size_t					 p_residueCount ) const
	{
		_findLineFromModel<ModelFieldLine>( p_model )->setRMSD( p_rmsd, p_residueCount );
	}
	void StructuralAlignmentModelListWidget::resetRMSD( const Model::BaseModel * const p_model ) const
	{
		_findLineFromModel<ModelFieldLine>( p_model )->resetRMSD();
	}

	void StructuralAlignmentModelListWidget::_initColumns()
	{
		_initColumn( int( GRID_LAYOUT_COLUMN::TICK ), "Static", 0 );
		_initColumn( int( GRID_LAYOUT_COLUMN::MODEL ), "Molecule", 1000 );
		_initColumn( int( GRID_LAYOUT_COLUMN::RMSD ), "RMSD", 0 );
		_initColumn( int( GRID_LAYOUT_COLUMN::REMOVE_BUTTON ), "", 0 );
	}

	CustomWidget::BaseModelFieldLine * const StructuralAlignmentModelListWidget::_instantiateLine()
	{
		ModelFieldLine * const newLine = new ModelFieldLine( this );
		return newLine;
	}
	void StructuralAlignmentModelListWidget::_initLine( CustomWidget::BaseModelFieldLine * const p_line,
														Model::BaseModel * const				 p_model ) const
	{
		CustomWidget::BaseModelListWidget::_initLine( p_line, p_model );

		ModelFieldLine * const castedLine = dynamic_cast<ModelFieldLine * const>( p_line );
		castedLine->setTickState( getModelCount() == 0 );
	}
	void StructuralAlignmentModelListWidget::_addLineInLayout( CustomWidget::BaseModelFieldLine * const p_line,
															   const int								p_row )
	{
		const ModelFieldLine * const castedLine = dynamic_cast<ModelFieldLine * const>( p_line );

		_addWidgetInColumn( castedLine->getTickButton(), p_row, int( GRID_LAYOUT_COLUMN::TICK ) );
		_addWidgetInColumn( castedLine->getModelField(), p_row, int( GRID_LAYOUT_COLUMN::MODEL ) );
		_addWidgetInColumn( castedLine->getRemoveButton(), p_row, int( GRID_LAYOUT_COLUMN::REMOVE_BUTTON ) );
		_addWidgetInColumn( castedLine->getRMSDLabel(), p_row, int( GRID_LAYOUT_COLUMN::RMSD ) );
	}
} // namespace VTX::UI::Widget::Analysis::StructuralAlignment
