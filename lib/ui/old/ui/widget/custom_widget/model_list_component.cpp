#include "ui/old_ui/ui/widget/custom_widget/model_list_component.hpp"
#include "ui/old_ui/ui/ui_action/self_referenced_action.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/util/ui.hpp"
#include <QRadioButton>
#include <QVBoxLayout>
#include <QVariant>
#include <app/old/mvc.hpp>
#include <app/old/event/global.hpp>
#include <app/old/core/scene/base_scene_item.hpp>

namespace VTX::UI::Widget::CustomWidget
{
	ModelListComponent::ModelListComponent( BaseModelListWidget * const p_modelList, QWidget * p_parent ) :
		BaseManualWidget( p_parent ), _modelListWidget( p_modelList )
	{
		_registerEvent( VTX::App::Old::Event::Global::SCENE_ITEM_REMOVED );
	}
	ModelListComponent::~ModelListComponent() {}

	void ModelListComponent::receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Old::Event::Global::SCENE_ITEM_REMOVED )
		{
			const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Core::Scene::BaseSceneItem *> & castedEvent
				= dynamic_cast<const VTX::App::Old::Core::Event::VTXEventArg<App::Old::Core::Scene::BaseSceneItem *> &>( p_event );

			App::Old::Core::Model::BaseModel & model
				= VTX::MVC_MANAGER().getModel<App::Old::Core::Model::BaseModel>( castedEvent.get()->getModelID() );
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

	void ModelListComponent::addModel( App::Old::Core::Model::BaseModel * const p_model ) { _modelListWidget->addModel( p_model ); }
	void ModelListComponent::insertModel( App::Old::Core::Model::BaseModel * const p_model, const int p_row )
	{
		_modelListWidget->insertModel( p_model, p_row );
	}
	void ModelListComponent::removeModel( App::Old::Core::Model::BaseModel * const p_model )
	{
		_modelListWidget->removeModel( p_model );
	}
	void ModelListComponent::swapModels( App::Old::Core::Model::BaseModel * const p_model1, App::Old::Core::Model::BaseModel * const p_model2 ) const
	{
		_modelListWidget->swapModels( p_model1, p_model2 );
	}
	void ModelListComponent::clearModels() { _modelListWidget->clearModels(); }

	bool ModelListComponent::hasModel( const App::Old::Core::Model::BaseModel * const p_model ) const
	{
		return _modelListWidget->hasModel( p_model );
	}
	int ModelListComponent::getModelCount() const { return _modelListWidget->getModelCount(); }

	std::vector<App::Old::Core::Model::BaseModel *> ModelListComponent::getModels() const { return _modelListWidget->getModels(); }

	void ModelListComponent::_emitModelListChangeEvent() { emit onModelListChange(); }
	void ModelListComponent::_onModelsDropped( std::vector<App::Old::Core::Model::BaseModel *> p_models )
	{
		for ( App::Old::Core::Model::BaseModel * const model : p_models )
			addModel( model );

		_foldButton->setFoldState( true );
	}

	void ModelListComponent::addTypeFilter( const App::Old::VTX_ID & p_typeID ) const
	{
		_dropArea->addTypeFilter( p_typeID );
		_modelListWidget->addTypeFilter( p_typeID );
	}

} // namespace VTX::UI::Widget::CustomWidget
