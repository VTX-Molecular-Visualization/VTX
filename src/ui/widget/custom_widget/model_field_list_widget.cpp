#include "model_field_list_widget.hpp"
#include "mvc/mvc_manager.hpp"
#include "ui/ui_action/self_referenced_action.hpp"
#include "ui/widget_factory.hpp"
#include "util/ui.hpp"
#include <QRadioButton>
#include <QVBoxLayout>
#include <QVariant>

namespace VTX::UI::Widget::CustomWidget
{
	ModelFieldListWidget::ModelFieldLine::ModelFieldLine( ModelFieldListWidget * const p_owner ) :
		_owner( p_owner ), QObject( p_owner )
	{
		_tickButton = new QRadioButton( _owner );

		_removeButton = new QPushButton( _owner );
		_removeButton->setIcon( Style::IconConst::get().REMOVE_LINE_ICON );
		_removeButton->setFlat( true );

		connect( _removeButton, &QPushButton::clicked, this, &ModelFieldLine::_callRemoveAction );

		_modelField = WidgetFactory::get().instantiateWidget<CustomWidget::ModelFieldWidget>( _owner, "modelField" );
		connect(
			_modelField, &CustomWidget::ModelFieldWidget::onModelChanged, this, &ModelFieldLine::_checkModelChange );
	}

	ModelFieldListWidget::ModelFieldLine::~ModelFieldLine() {}

	void ModelFieldListWidget::ModelFieldLine::destroyWidgets()
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
	}

	void ModelFieldListWidget::ModelFieldLine::setModel( Model::BaseModel * const p_model )
	{
		_modelField->setModel( p_model );
	}

	bool ModelFieldListWidget::ModelFieldLine::isTicked() const { return _tickButton->isChecked(); }
	void ModelFieldListWidget::ModelFieldLine::setTickState( const bool p_ticked )
	{
		_tickButton->setChecked( p_ticked );
	}

	void ModelFieldListWidget::ModelFieldLine::_callRemoveAction()
	{
		Model::BaseModel * const model = _modelField->getModel();
		_owner->removeModel( model );
	}
	void ModelFieldListWidget::ModelFieldLine::_checkModelChange( Model::BaseModel * const p_model )
	{
		if ( _owner->getContainsOnlyUniqueModel() && _owner->hasModel( p_model ) )
			_owner->swapModels( _modelField->getModel(), p_model );
	}

	ModelFieldListWidget::ModelFieldListWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}
	ModelFieldListWidget::~ModelFieldListWidget() {}

	void ModelFieldListWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QVBoxLayout * const mainLayout = new QVBoxLayout( this );
		mainLayout->setContentsMargins( 0, 0, 0, 0 );
		QWidget * const modelListWidget = new QWidget( this );

		_dropArea = WidgetFactory::get().instantiateWidget<CustomWidget::ModelDropArea>( this, "dropArea" );
		_dropArea->setSizePolicy( QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum );
		_dropArea->setFixedHeight( 32 );
		_dropArea->setFrameShadow( QFrame::Shadow::Sunken );
		_dropArea->addTypeFilter( ID::Model::MODEL_MOLECULE );
		_dropArea->setAcceptGroup( true );

		_foldButton = WidgetFactory::get().instantiateWidget<CustomWidget::FoldingButton>(
			this, modelListWidget, "foldButton" );

		_modelListLayout = new QGridLayout( modelListWidget );
		_modelListLayout->setContentsMargins( 0, 0, 0, 0 );
		_modelListLayout->setSpacing( 10 );

		QLabel * const layoutHeadingStaticMol = new QLabel( modelListWidget );
		layoutHeadingStaticMol->setText( "Static" );
		QLabel * const layoutHeadingMolecule = new QLabel( modelListWidget );
		layoutHeadingMolecule->setText( "Molecule" );

		_modelListLayout->setColumnStretch( int( GRID_LAYOUT_COLUMN::TICK ), 0 );
		_modelListLayout->addWidget( layoutHeadingStaticMol, 0, int( GRID_LAYOUT_COLUMN::TICK ) );

		_modelListLayout->setColumnStretch( int( GRID_LAYOUT_COLUMN::MODEL ), 1000 );
		_modelListLayout->addWidget( layoutHeadingMolecule, 0, int( GRID_LAYOUT_COLUMN::MODEL ) );

		_modelListLayout->setColumnStretch( int( GRID_LAYOUT_COLUMN::REMOVE_BUTTON ), 0 );

		mainLayout->addWidget( _foldButton );
		mainLayout->addWidget( _dropArea );
		mainLayout->addStretch( 1000 );
	}

	void ModelFieldListWidget::_setupSlots()
	{
		connect(
			_dropArea, &CustomWidget::ModelDropArea::onModelsDropped, this, &ModelFieldListWidget::_onModelsDropped );
	}

	void ModelFieldListWidget::localize() {};

	void ModelFieldListWidget::setContainsOnlyUniqueModel( const bool p_containsOnlyUniqueModel )
	{
		_containsOnlyUniqueModel = p_containsOnlyUniqueModel;
	}

	bool ModelFieldListWidget::hasModel( const Model::BaseModel * const p_model ) const
	{
		for ( const ModelFieldLine * const line : _lines )
			if ( line->getModel() == p_model )
				return true;

		return false;
	}

	int								ModelFieldListWidget::getModelCount() const { return int( _lines.size() ); }
	std::vector<Model::BaseModel *> ModelFieldListWidget::getModels() const
	{
		std::vector<Model::BaseModel *> res = std::vector<Model::BaseModel *>();
		res.reserve( _lines.size() );

		for ( const ModelFieldLine * const line : _lines )
			res.emplace_back( line->getModel() );

		return res;
	}

	void ModelFieldListWidget::setTitle( const QString & p_title ) const { _foldButton->setTitle( p_title ); }

	void ModelFieldListWidget::addModel( Model::BaseModel * const p_model )
	{
		// Don't add a model already present.
		if ( _isModelAlreadyInList( p_model ) )
			return;

		_addModelInLayout( p_model, _modelListLayout->rowCount() );

		emit onModelListChange();
	}
	void ModelFieldListWidget::insertModel( Model::BaseModel * const p_model, const int p_position ) {}
	void ModelFieldListWidget::removeModel( Model::BaseModel * const p_model )
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
				break;
			}

			it++;
		}
	}

	void ModelFieldListWidget::swapModels( Model::BaseModel * const p_model1, Model::BaseModel * const p_model2 ) const
	{
		bool firstSwapDone = false;

		for ( const ModelFieldLine * const line : _lines )
		{
			if ( line->getModel() == p_model1 )
			{
				CustomWidget::ModelFieldWidget * const modelField = line->getModelField();

				const bool oldSignalState = modelField->blockSignals( true );
				modelField->setModel( p_model2 );
				modelField->blockSignals( oldSignalState );

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

				if ( firstSwapDone )
					break;
				else
					firstSwapDone = true;
			}
		}
	}

	Model::BaseModel * const ModelFieldListWidget::getTickedModel() const
	{
		for ( ModelFieldLine * const line : _lines )
			if ( line->isTicked() )
				return line->getModel();

		return nullptr;
	}

	std::vector<Model::BaseModel *> const ModelFieldListWidget::getNotTickedModels() const
	{
		std::vector<Model::BaseModel *> res = std::vector<Model::BaseModel *>();
		res.reserve( _lines.size() );

		for ( ModelFieldLine * const line : _lines )
			if ( !line->isTicked() )
				res.emplace_back( line->getModel() );

		res.shrink_to_fit();

		return res;
	}

	ModelFieldListWidget::ModelFieldLine * ModelFieldListWidget::_findLineFromModel(
		Model::BaseModel * const p_model ) const
	{
		for ( ModelFieldLine * const line : _lines )
			if ( line->getModel() == p_model )
				return line;

		return nullptr;
	}

	ModelFieldListWidget::ModelFieldLine * ModelFieldListWidget::_findNextLine(
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

	bool ModelFieldListWidget::_isModelAlreadyInList( Model::BaseModel * const p_model ) const
	{
		for ( const ModelFieldLine * const line : _lines )
			if ( line->getModel() == p_model )
				return true;

		return false;
	}

	void ModelFieldListWidget::_addModelInLayout( Model::BaseModel * const p_model, const int p_row )
	{
		ModelFieldLine * const newLine = new ModelFieldLine( this );

		newLine->setModel( p_model );
		newLine->setTickState( getModelCount() == 0 );

		_modelListLayout->addWidget( newLine->getTickButton(), p_row, 0 );
		_modelListLayout->addWidget( newLine->getModelField(), p_row, 1 );
		_modelListLayout->addWidget( newLine->getRemoveButton(), p_row, 2 );

		_lines.emplace_back( newLine );
	}

	void ModelFieldListWidget::_onModelsDropped( std::vector<Model::BaseModel *> p_models )
	{
		for ( Model::BaseModel * const model : p_models )
			addModel( model );

		_foldButton->setFoldState( true );
	}

} // namespace VTX::UI::Widget::CustomWidget
