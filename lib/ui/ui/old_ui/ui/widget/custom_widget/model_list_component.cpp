#include "model_list_component.hpp"
#include "old_ui/ui/ui_action/self_referenced_action.hpp"
#include "old_ui/ui/widget_factory.hpp"
#include "old_ui/util/ui.hpp"
#include <QRadioButton>
#include <QVBoxLayout>
#include <QVariant>
#include <app/old_app/generic/base_scene_item.hpp>
#include <app/old_app/mvc/mvc_manager.hpp>

namespace VTX::UI::Widget::CustomWidget
{
	ModelListComponent::ModelListComponent( BaseModelListWidget * const p_modelList, QWidget * p_parent ) :
		BaseManualWidget( p_parent ), _modelListWidget( p_modelList )
	{
		_registerEvent( VTX::Event::Global::SCENE_ITEM_REMOVED );
	}
	ModelListComponent::~ModelListComponent() {}

	void ModelListComponent::receiveEvent( const VTX::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::Event::Global::SCENE_ITEM_REMOVED )
		{
			const VTX::Event::VTXEventPtr<Generic::BaseSceneItem> & castedEvent
				= dynamic_cast<const VTX::Event::VTXEventPtr<Generic::BaseSceneItem> &>( p_event );

			Model::BaseModel & model
				= MVC::MvcManager::get().getModel<Model::BaseModel>( castedEvent.ptr->getModelID() );
			removeModel( &model );
		}
	}

	void ModelListComponent::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QVBoxLayout * const mainLayout = new QVBoxLayout( this );
		mainLayout->setContentsMargins( 0, 0, 0, 0 );

		_dropArea = WidgetFactory::get().instantiateWidget<CustomWidget::TextualModelDropArea>( this, "dropArea" );
		_dropArea->setSizePolicy( QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed );
		_dropArea->setFixedHeight( 36 );
		_dropArea->setAcceptGroup( true );

		_foldButton = WidgetFactory::get().instantiateWidget<CustomWidget::FoldingButton>(
			this, _modelListWidget, "foldButton" );

		mainLayout->addWidget( _foldButton );
		mainLayout->addWidget( _dropArea );
		mainLayout->addStretch( 1000 );
	}

	void ModelListComponent::_setupSlots()
	{
		connect(
			_dropArea, &CustomWidget::ModelDropArea::onModelsDropped, this, &ModelListComponent::_onModelsDropped );

		connect( _modelListWidget,
				 &BaseModelListWidget::onModelListChange,
				 this,
				 &ModelListComponent::_emitModelListChangeEvent );
	}

	void ModelListComponent::localize() {};

	void ModelListComponent::setContainsOnlyUniqueModel( const bool p_containsOnlyUniqueModel ) const
	{
		_modelListWidget->setContainsOnlyUniqueModel( p_containsOnlyUniqueModel );
	}

	void ModelListComponent::setTitle( const QString & p_title ) const { _foldButton->setTitle( p_title ); }
	void ModelListComponent::setFoldState( const bool p_expanded ) const { _foldButton->setFoldState( p_expanded ); }

	void ModelListComponent::addModel( Model::BaseModel * const p_model ) { _modelListWidget->addModel( p_model ); }
	void ModelListComponent::insertModel( Model::BaseModel * const p_model, const int p_row )
	{
		_modelListWidget->insertModel( p_model, p_row );
	}
	void ModelListComponent::removeModel( Model::BaseModel * const p_model )
	{
		_modelListWidget->removeModel( p_model );
	}
	void ModelListComponent::swapModels( Model::BaseModel * const p_model1, Model::BaseModel * const p_model2 ) const
	{
		_modelListWidget->swapModels( p_model1, p_model2 );
	}
	void ModelListComponent::clearModels() { _modelListWidget->clearModels(); }

	bool ModelListComponent::hasModel( const Model::BaseModel * const p_model ) const
	{
		return _modelListWidget->hasModel( p_model );
	}
	int ModelListComponent::getModelCount() const { return _modelListWidget->getModelCount(); }

	std::vector<Model::BaseModel *> ModelListComponent::getModels() const { return _modelListWidget->getModels(); }

	void ModelListComponent::_emitModelListChangeEvent() { emit onModelListChange(); }
	void ModelListComponent::_onModelsDropped( std::vector<Model::BaseModel *> p_models )
	{
		for ( Model::BaseModel * const model : p_models )
			addModel( model );

		_foldButton->setFoldState( true );
	}

	void ModelListComponent::addTypeFilter( const ID::VTX_ID & p_typeID ) const
	{
		_dropArea->addTypeFilter( p_typeID );
		_modelListWidget->addTypeFilter( p_typeID );
	}

} // namespace VTX::UI::Widget::CustomWidget
