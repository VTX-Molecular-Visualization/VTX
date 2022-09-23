#include "structural_alignment_model_list_widget.hpp"
#include "mvc/mvc_manager.hpp"
#include "style.hpp"
#include "ui/ui_action/self_referenced_action.hpp"
#include "ui/widget_factory.hpp"
#include "util/ui.hpp"
#include <QRadioButton>
#include <QVBoxLayout>
#include <QVariant>

namespace VTX::UI::Widget::Analysis::StructuralAlignment
{
	StructuralAlignmentModelListWidget::ModelFieldLine::ModelFieldLine(
		StructuralAlignmentModelListWidget * const p_owner ) :
		_owner( p_owner ),
		QObject( p_owner )
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
	StructuralAlignmentModelListWidget::ModelFieldLine::~ModelFieldLine() {}

	void StructuralAlignmentModelListWidget::ModelFieldLine::destroyWidgets()
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
	void StructuralAlignmentModelListWidget::ModelFieldLine::setRMSD( const float p_rmsd )
	{
		_rmsdLabel->setText( QString::fromStdString( std::to_string( p_rmsd ) ) );
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

	StructuralAlignmentModelListWidget::StructuralAlignmentModelListWidget( QWidget * p_parent ) :
		CustomWidget::BaseModelListWidget( p_parent )
	{
	}

	void StructuralAlignmentModelListWidget::_setupUi( const QString & p_name )
	{
		_layout = new QGridLayout( this );

		_layout->setContentsMargins( 0, 0, 0, 0 );
		_layout->setSpacing( 10 );

		_initColumn( GRID_LAYOUT_COLUMN::TICK, "Static", 0 );
		_initColumn( GRID_LAYOUT_COLUMN::MODEL, "Molecule", 1000 );
		_initColumn( GRID_LAYOUT_COLUMN::REMOVE_BUTTON, "", 0 );
		_initColumn( GRID_LAYOUT_COLUMN::RMSD, "RMSD", 0 );
	}
	void StructuralAlignmentModelListWidget::_setupSlots() {}
	void StructuralAlignmentModelListWidget::localize() {}

	void StructuralAlignmentModelListWidget::addModel( Model::BaseModel * const p_model )
	{
		if ( getContainsOnlyUniqueModel() && _isModelAlreadyInList( p_model ) )
			return;

		_addModelInLayout( p_model, getModelCount() );

		emit onModelListChange();
	}
	void StructuralAlignmentModelListWidget::insertModel( Model::BaseModel * const p_model, const int p_row )
	{
		bool listHasChanged = false;

		if ( getContainsOnlyUniqueModel() && _isModelAlreadyInList( p_model ) )
		{
			// TODO
			//_modelListWidget->moveModel( p_model, p_position );
		}
		else
		{
			_addModelInLayout( p_model, getModelCount() );
			listHasChanged = true;
		}

		if ( listHasChanged )
			emit onModelListChange();
	}
	void StructuralAlignmentModelListWidget::removeModel( Model::BaseModel * const p_model )
	{
		std::vector<ModelFieldLine *>::const_iterator it = _lines.begin();

		while ( it != _lines.end() )
		{
			if ( ( *it )->getModel() == p_model )
			{
				ModelFieldLine * const lineToDestroy = *it;
				if ( lineToDestroy->isTicked() )
				{
					ModelFieldLine * const lineToCheck = _findNextLine( lineToDestroy );
					if ( lineToCheck != nullptr )
						lineToCheck->setTickState( true );
				}

				lineToDestroy->destroyWidgets();

				delete lineToDestroy;
				it = _lines.erase( it );

				emit onModelListChange();
				// break;
			}
			else
			{
				it++;
			}
		}
	}
	void StructuralAlignmentModelListWidget::swapModels( Model::BaseModel * const p_model1,
														 Model::BaseModel * const p_model2 ) const
	{
		bool firstSwapDone = false;

		for ( ModelFieldLine * const line : _lines )
		{
			if ( line->getModel() == p_model1 )
			{
				CustomWidget::ModelFieldWidget * const modelField = line->getModelField();

				const bool oldSignalState = modelField->blockSignals( true );
				modelField->setModel( p_model2 );
				modelField->blockSignals( oldSignalState );
				line->resetRMSD();

				if ( firstSwapDone )
					break;
				else
					firstSwapDone = true;
			}
			else if ( line->getModel() == p_model2 )
			{
				CustomWidget::ModelFieldWidget * const modelField = line->getModelField();

				const bool oldSignalState = modelField->blockSignals( true );
				modelField->setModel( p_model1 );
				modelField->blockSignals( oldSignalState );
				line->resetRMSD();

				if ( firstSwapDone )
					break;
				else
					firstSwapDone = true;
			}
		}
	}

	bool StructuralAlignmentModelListWidget::hasModel( const Model::BaseModel * const p_model ) const
	{
		for ( const ModelFieldLine * const line : _lines )
			if ( line->getModel() == p_model )
				return true;

		return false;
	}
	int StructuralAlignmentModelListWidget::getModelCount() const { return int( _lines.size() ); }
	std::vector<Model::BaseModel *> StructuralAlignmentModelListWidget::getModels() const
	{
		std::vector<Model::BaseModel *> res = std::vector<Model::BaseModel *>();
		res.reserve( _lines.size() );

		for ( const ModelFieldLine * const line : _lines )
			res.emplace_back( line->getModel() );

		return res;
	}

	Model::BaseModel * const StructuralAlignmentModelListWidget::getTickedModel() const
	{
		for ( ModelFieldLine * const line : _lines )
			if ( line->isTicked() )
				return line->getModel();

		return nullptr;
	}
	std::vector<Model::BaseModel *> StructuralAlignmentModelListWidget::getNotTickedModels() const
	{
		std::vector<Model::BaseModel *> res = std::vector<Model::BaseModel *>();
		res.reserve( _lines.size() );

		for ( ModelFieldLine * const line : _lines )
			if ( !line->isTicked() )
				res.emplace_back( line->getModel() );

		res.shrink_to_fit();

		return res;
	}

	void StructuralAlignmentModelListWidget::setRMSD( const Model::BaseModel * const p_model, const float p_rmsd ) const
	{
		_findLineFromModel( p_model )->setRMSD( p_rmsd );
	}
	void StructuralAlignmentModelListWidget::resetRMSD( const Model::BaseModel * const p_model ) const
	{
		_findLineFromModel( p_model )->resetRMSD();
	}

	void StructuralAlignmentModelListWidget::_initColumn( const GRID_LAYOUT_COLUMN p_column,
														  const std::string		   p_title,
														  const int				   p_stretch )
	{
		const int columnIndex = int( p_column );
		_layout->setColumnStretch( columnIndex, p_stretch );

		if ( !p_title.empty() )
		{
			QLabel * const titleLabel = new QLabel( this );
			titleLabel->setText( QString::fromStdString( p_title ) );

			_layout->addWidget( titleLabel, 0, columnIndex );
		}
	}
	void StructuralAlignmentModelListWidget::_addModelInLayout( Model::BaseModel * const p_model, const int p_row )
	{
		ModelFieldLine * const newLine = new ModelFieldLine( this );

		const int realRow = p_row + 1;

		_layout->addWidget( newLine->getTickButton(), realRow, int( GRID_LAYOUT_COLUMN::TICK ) );
		_layout->addWidget( newLine->getModelField(), realRow, int( GRID_LAYOUT_COLUMN::MODEL ) );
		_layout->addWidget( newLine->getRemoveButton(), realRow, int( GRID_LAYOUT_COLUMN::REMOVE_BUTTON ) );
		_layout->addWidget( newLine->getRMSDLabel(), realRow, int( GRID_LAYOUT_COLUMN::RMSD ) );

		newLine->setModel( p_model );
		newLine->setTickState( getModelCount() == 0 );

		_lines.emplace_back( newLine );
	}

	bool StructuralAlignmentModelListWidget::_isModelAlreadyInList( const Model::BaseModel * const p_model ) const
	{
		for ( const ModelFieldLine * const line : _lines )
			if ( line->getModel() == p_model )
				return true;

		return false;
	}

	StructuralAlignmentModelListWidget::ModelFieldLine * StructuralAlignmentModelListWidget::_findLineFromModel(
		const Model::BaseModel * const p_model ) const
	{
		for ( ModelFieldLine * const line : _lines )
			if ( line->getModel() == p_model )
				return line;

		return nullptr;
	}
	StructuralAlignmentModelListWidget::ModelFieldLine * StructuralAlignmentModelListWidget::_findNextLine(
		const ModelFieldLine * const p_from ) const
	{
		ModelFieldLine * lineToCheck = nullptr;

		for ( int i = 0; i < _lines.size(); i++ )
		{
			if ( _lines[ i ] == p_from )
			{
				if ( i < _lines.size() - 1 )
				{
					lineToCheck = _lines[ i + 1 ];
				}
				else if ( i > 0 )
				{
					lineToCheck = _lines[ i - 1 ];
				}
				else
				{
					lineToCheck = nullptr;
				}

				break;
			}
		}

		return lineToCheck;
	}

} // namespace VTX::UI::Widget::Analysis::StructuralAlignment
