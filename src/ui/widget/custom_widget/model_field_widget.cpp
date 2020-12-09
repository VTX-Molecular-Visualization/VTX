#include "model_field_widget.hpp"
#include "id.hpp"
#include "mvc/mvc_manager.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace CustomWidget
			{
				void ModelFieldWidget::_setupUi( const QString & p_name )
				{
					setObjectName( p_name );
					setAcceptDrops( true );

					setFixedSize( 200, 50 );
				}

				void ModelFieldWidget::_setupSlots() {}

				void ModelFieldWidget::localize() {};

				void ModelFieldWidget::dragEnterEvent( QDragEnterEvent * event )
				{
					if ( event->mimeData()->hasFormat( "custom/representable" ) )
						event->acceptProposedAction();
				}
				void ModelFieldWidget::dropEvent( QDropEvent * event )
				{
					const QByteArray byteData		   = event->mimeData()->data( "custom/representable" );
					const Model::ID idDroppedObject = std::atoi( byteData.data() );

					Model::BaseModel & model = MVC::MvcManager::get().getModel<Model::BaseModel>( idDroppedObject );

					event->acceptProposedAction();

					if ( _model != &model )
					{
						_model = &model;
						emit dataChanged();
					}
				}
			} // namespace CustomWidget
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
