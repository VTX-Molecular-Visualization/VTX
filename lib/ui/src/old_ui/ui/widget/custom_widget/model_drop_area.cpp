#include "ui/old_ui/ui/widget/custom_widget/model_drop_area.hpp"
#include "ui/old_ui/ui/mime_type.hpp"
#include <app/old/application/selection/selection.hpp>
#include <app/old/id.hpp>
#include <app/old/mvc.hpp>
#include <set>

namespace VTX::UI::Widget::CustomWidget
{
	void ModelDropArea::copyTypeFilters( const ModelDropArea & p_from, ModelDropArea & p_to )
	{
		for ( const App::Old::VTX_ID & id : p_from._filters )
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

	void ModelDropArea::addTypeFilter( const VTX::App::Old::VTX_ID & p_modelID ) { _filters.emplace_back( p_modelID ); }
	bool ModelDropArea::_hasFiltersToCheck() const { return _filters.size() > 0; }
	bool ModelDropArea::matchFilter( const VTX::App::Old::VTX_ID & p_type ) const
	{
		return !_hasFiltersToCheck() || std::find( _filters.begin(), _filters.end(), p_type ) != _filters.end();
	}
	bool ModelDropArea::matchFilter( const App::Old::Core::Model::BaseModel & p_model ) const
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
				const App::Old::VTX_ID &			  typeId	= modelData.getTypeID();

				// For a selection, accept drop only if all models of the selection are accepted by the filter
				if ( typeId == App::Old::ID::Model::MODEL_SELECTION )
				{
					const App::Old::Application::Selection::SelectionModel & selection
						= VTX::MVC_MANAGER().getModel<App::Old::Application::Selection::SelectionModel>(
							modelData.getModelID() );

					if ( _acceptGroup )
					{
						std::set<App::Old::VTX_ID> selectionTypeIDs = std::set<App::Old::VTX_ID>();
						selection.getItemTypes( selectionTypeIDs );

						for ( const App::Old::VTX_ID & typeId : selectionTypeIDs )
						{
							accept = accept && matchFilter( typeId );
						}
					}
					else if ( selection.getItems().size() == 1 )
					{
						std::set<App::Old::VTX_ID> selectionTypeIDs = std::set<App::Old::VTX_ID>();
						selection.getItemTypes( selectionTypeIDs );

						for ( const App::Old::VTX_ID & typeId : selectionTypeIDs )
						{
							accept = accept && matchFilter( typeId );
						}
					}
					else
					{
						accept = false;
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
		App::Old::Core::Model::BaseModel & model
			= VTX::MVC_MANAGER().getModel<App::Old::Core::Model::BaseModel>( modelData.getModelID() );

		p_event->acceptProposedAction();

		if ( model.getTypeId() == App::Old::ID::Model::MODEL_SELECTION )
		{
			const App::Old::Application::Selection::SelectionModel & selection
				= VTX::MVC_MANAGER().getModel<App::Old::Application::Selection::SelectionModel>( modelData.getModelID() );

			std::set<App::Old::VTX_ID> types = std::set<App::Old::VTX_ID>();
			selection.getItemTypes( types );

			std::set<App::Old::Core::Model::BaseModel *> droppedModels = std::set<App::Old::Core::Model::BaseModel *>();

			for ( const App::Old::VTX_ID & typeID : types )
			{
				if ( matchFilter( typeID ) )
				{
					selection.getItemsOfType<App::Old::Core::Model::BaseModel>( typeID, droppedModels );
				}
			}

			std::vector<App::Old::Core::Model::BaseModel *> droppedModelsVector
				= std::vector<App::Old::Core::Model::BaseModel *>();
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
