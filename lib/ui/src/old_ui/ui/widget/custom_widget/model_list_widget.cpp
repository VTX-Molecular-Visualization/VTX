#include "ui/old_ui/ui/widget/custom_widget/model_list_widget.hpp"
#include "ui/old_ui/ui/ui_action/self_referenced_action.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/util/ui.hpp"
#include <QLabel>
#include <app/old_app/mvc/mvc_manager.hpp>

namespace VTX::UI::Widget::CustomWidget
{
	BaseModelFieldLine::BaseModelFieldLine( BaseModelListWidget * const p_parent ) : QObject( p_parent ) {}

	BaseModelListWidget::BaseModelListWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

	void BaseModelListWidget::_setupUi( const QString & p_name )
	{
		_layout = new QGridLayout( this );

		_layout->setContentsMargins( 0, 0, 0, 0 );
		_layout->setSpacing( 10 );

		_initColumns();
	}
	void BaseModelListWidget::_setupSlots() {}
	void BaseModelListWidget::localize() {}

	void BaseModelListWidget::addModel( Model::BaseModel * const p_model )
	{
		if ( getContainsOnlyUniqueModel() && _isModelAlreadyInList( p_model ) )
			return;

		_addModelInLayout( p_model, getModelCount() );

		emit onModelListChange();
	}
	void BaseModelListWidget::insertModel( Model::BaseModel * const p_model, const int p_row )
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
	void BaseModelListWidget::removeModel( Model::BaseModel * const p_model )
	{
		std::vector<BaseModelFieldLine *>::const_iterator it = _lines.begin();

		while ( it != _lines.end() )
		{
			if ( ( *it )->getModel() == p_model )
			{
				_onLineWillBeRemoved( *it );

				BaseModelFieldLine * const lineToDestroy = *it;

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

	void BaseModelListWidget::clearModels()
	{
		while ( getModels().size() > 0 )
		{
			removeModel( getModels()[ 0 ] );
		}
	}

	void BaseModelListWidget::swapModels( Model::BaseModel * const p_model1, Model::BaseModel * const p_model2 ) const
	{
		bool firstSwapDone = false;

		for ( BaseModelFieldLine * const line : _lines )
		{
			if ( line->getModel() == p_model1 )
			{
				const bool oldSignalState = line->blockSignals( true );
				line->setModel( p_model2 );
				line->blockSignals( oldSignalState );

				if ( firstSwapDone )
					break;
				else
					firstSwapDone = true;
			}
			else if ( line->getModel() == p_model2 )
			{
				const bool oldSignalState = line->blockSignals( true );
				line->setModel( p_model1 );
				line->blockSignals( oldSignalState );

				if ( firstSwapDone )
					break;
				else
					firstSwapDone = true;
			}
		}
	}

	bool BaseModelListWidget::hasModel( const Model::BaseModel * const p_model ) const
	{
		for ( const BaseModelFieldLine * const line : _lines )
			if ( line->getModel() == p_model )
				return true;

		return false;
	}
	int								BaseModelListWidget::getModelCount() const { return int( _lines.size() ); }
	std::vector<Model::BaseModel *> BaseModelListWidget::getModels() const
	{
		std::vector<Model::BaseModel *> res = std::vector<Model::BaseModel *>();
		res.reserve( _lines.size() );

		for ( const BaseModelFieldLine * const line : _lines )
			res.emplace_back( line->getModel() );

		return res;
	}

	void BaseModelListWidget::setContainsOnlyUniqueModel( const bool p_containsOnlyUniqueModel )
	{
		_containsOnlyUniqueModel = p_containsOnlyUniqueModel;
	}
	void BaseModelListWidget::addTypeFilter( const ID::VTX_ID & p_typeID ) { _filters.emplace_back( p_typeID ); }

	bool BaseModelListWidget::_isModelAlreadyInList( const Model::BaseModel * const p_model ) const
	{
		for ( const BaseModelFieldLine * const line : _lines )
			if ( line->getModel() == p_model )
				return true;

		return false;
	}

	void BaseModelListWidget::_addModelInLayout( Model::BaseModel * const p_model, const int p_row )
	{
		BaseModelFieldLine * const newLine = _instantiateLine();
		_initLine( newLine, p_model );
		_addLineInLayout( newLine, p_row );

		_lines.emplace_back( newLine );
	}

	void BaseModelListWidget::_addWidgetInColumn( QWidget * p_widget, const int p_row, const int p_column )
	{
		// Add 1 for title row
		_layout->addWidget( p_widget, p_row + 1, p_column );
	}

	void BaseModelListWidget::_initLine( BaseModelFieldLine * const p_line, Model::BaseModel * const p_model ) const
	{
		p_line->setModel( p_model );
	}

	void BaseModelListWidget::_initColumn( const int p_columnIndex, const std::string p_title, const int p_stretch )
	{
		_layout->setColumnStretch( p_columnIndex, p_stretch );

		if ( !p_title.empty() )
		{
			QLabel * const titleLabel = new QLabel( this );
			titleLabel->setText( QString::fromStdString( p_title ) );

			_layout->addWidget( titleLabel, 0, p_columnIndex );
		}
	}

	BaseModelFieldLine * BaseModelListWidget::_findLineFromModel( const Model::BaseModel * const p_model ) const
	{
		for ( BaseModelFieldLine * const line : _lines )
			if ( line->getModel() == p_model )
				return line;

		return nullptr;
	}
	BaseModelFieldLine * BaseModelListWidget::_findNextLine( const BaseModelFieldLine * const p_from ) const
	{
		BaseModelFieldLine * nextLine = nullptr;

		for ( int i = 0; i < _lines.size(); i++ )
		{
			if ( _lines[ i ] == p_from )
			{
				if ( i < _lines.size() - 1 )
				{
					nextLine = _lines[ i + 1 ];
				}
				else if ( i > 0 )
				{
					nextLine = _lines[ i - 1 ];
				}
				else
				{
					nextLine = nullptr;
				}

				break;
			}
		}

		return nextLine;
	}
} // namespace VTX::UI::Widget::CustomWidget
