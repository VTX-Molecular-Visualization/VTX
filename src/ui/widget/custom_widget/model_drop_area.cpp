#include "model_drop_area.hpp"
#include "id.hpp"
#include "mvc/mvc_manager.hpp"
#include "ui/mime_type.hpp"

namespace VTX::UI::Widget::CustomWidget
{
	ModelDropArea::ModelDropArea( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};

	void ModelDropArea::_setupUi( const QString & p_name )
	{
		setObjectName( p_name );
		setAcceptDrops( true );
	}

	void ModelDropArea::_setupSlots() {}

	void ModelDropArea::localize() {};

	void ModelDropArea::addTypeFilter( const VTX::ID::VTX_ID & p_modelID ) { _filters.emplace_back( p_modelID ); }

	void ModelDropArea::dragEnterEvent( QDragEnterEvent * event )
	{
		if ( event->mimeData()->hasFormat(
				 UI::MimeType::getQStringMimeType( UI::MimeType::ApplicationMimeType::BASE_MODEL ) ) )
			event->acceptProposedAction();
	}
	void ModelDropArea::dropEvent( QDropEvent * event )
	{
		const QByteArray byteData = event->mimeData()->data(
			UI::MimeType::getQStringMimeType( UI::MimeType::ApplicationMimeType::BASE_MODEL ) );
		const Model::ID idDroppedObject = std::atoi( byteData.data() );

		Model::BaseModel & model = MVC::MvcManager::get().getModel<Model::BaseModel>( idDroppedObject );

		event->acceptProposedAction();

		emit onModelDropped( &model );
	}

} // namespace VTX::UI::Widget::CustomWidget
