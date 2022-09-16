#include "model_drop_area.hpp"
#include "id.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "ui/mime_type.hpp"
#include <set>

namespace VTX::UI::Widget::CustomWidget
{
	void ModelDropArea::copyTypeFilters( const ModelDropArea & p_from, ModelDropArea & p_to )
	{
		for ( const ID::VTX_ID & id : p_from._filters )
		{
			p_to.addTypeFilter( id );
		}
	}

	ModelDropArea::ModelDropArea( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};

	void ModelDropArea::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		setAcceptDrops( true );
	}

	void ModelDropArea::_setupSlots() {}

	void ModelDropArea::localize() {};

	void ModelDropArea::setAcceptGroup( const bool p_accept ) { _acceptGroup = p_accept; }

	void ModelDropArea::addTypeFilter( const VTX::ID::VTX_ID & p_modelID ) { _filters.emplace_back( p_modelID ); }
	bool ModelDropArea::_hasFiltersToCheck() const { return _filters.size() > 0; }
	bool ModelDropArea::matchFilter( const VTX::ID::VTX_ID & p_type ) const
	{
		return !_hasFiltersToCheck() || std::find( _filters.begin(), _filters.end(), p_type ) != _filters.end();
	}
	bool ModelDropArea::matchFilter( const Model::BaseModel & p_model ) const
	{
		return matchFilter( p_model.getTypeId() );
	}

	void ModelDropArea::dragEnterEvent( QDragEnterEvent * p_event )
	{
		const bool dragModel
			= UI::MimeType::checkApplicationDataType( p_event->mimeData(), UI::MimeType::ApplicationMimeType::MODEL );

		if ( dragModel )
		{
			bool accept = true;

			if ( _hasFiltersToCheck() )
			{
				const UI::MimeType::ModelData modelData = UI::MimeType::getModelData( p_event->mimeData() );
				const ID::VTX_ID &			  typeId	= modelData.getTypeID();

				// For a selection, accept drop only if all models of the selection are accepted by the filter
				if ( _acceptGroup && typeId == ID::Model::MODEL_SELECTION )
				{
					const Model::Selection & selection
						= MVC::MvcManager::get().getModel<Model::Selection>( modelData.getModelID() );

					std::set<ID::VTX_ID> selectionTypeIDs = std::set<ID::VTX_ID>();
					selection.getItemTypes( selectionTypeIDs );

					for ( const ID::VTX_ID & typeId : selectionTypeIDs )
					{
						accept = accept && matchFilter( typeId );
					}
				}
				else
				{
					accept = matchFilter( modelData.getTypeID() );
				}
			}

			if ( accept )
				p_event->acceptProposedAction();
		}
	}
	void ModelDropArea::dropEvent( QDropEvent * p_event )
	{
		const UI::MimeType::ModelData modelData = UI::MimeType::getModelData( p_event->mimeData() );
		Model::BaseModel & model = MVC::MvcManager::get().getModel<Model::BaseModel>( modelData.getModelID() );

		p_event->acceptProposedAction();

		if ( model.getTypeId() == ID::Model::MODEL_SELECTION )
		{
			const Model::Selection & selection
				= MVC::MvcManager::get().getModel<Model::Selection>( modelData.getModelID() );

			std::set<ID::VTX_ID> types = std::set<ID::VTX_ID>();
			selection.getItemTypes( types );

			std::set<Model::BaseModel *> droppedModels = std::set<Model::BaseModel *>();

			for ( const ID::VTX_ID & typeID : types )
			{
				if ( matchFilter( typeID ) )
				{
					selection.getItemsOfType<Model::BaseModel>( typeID, droppedModels );
				}
			}

			std::vector<Model::BaseModel *> droppedModelsVector = std::vector<Model::BaseModel *>();
			droppedModelsVector.resize( droppedModels.size() );
			std::move( droppedModels.begin(), droppedModels.end(), droppedModelsVector.begin() );

			if ( isAcceptingGroup() )
				emit onModelsDropped( droppedModelsVector );
			else if ( droppedModelsVector.size() == 1 )
				emit onModelDropped( droppedModelsVector[ 0 ] );
		}
		else
		{
			if ( isAcceptingGroup() )
				emit onModelsDropped( { &model } );
			else
				emit onModelDropped( &model );
		}
	}

} // namespace VTX::UI::Widget::CustomWidget
