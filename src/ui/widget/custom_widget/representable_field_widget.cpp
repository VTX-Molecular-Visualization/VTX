#include "representable_field_widget.hpp"
#include "id.hpp"
#include "model/base_model.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "mvc/mvc_manager.hpp"
#include "ui/mime_type.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace CustomWidget
			{
				void RepresentableFieldWidget::_setupUi( const QString & p_name )
				{
					setObjectName( p_name );
					setAcceptDrops( true );

					setText( p_name );
					setFrameStyle( QFrame::Panel | QFrame::Sunken );
				}

				void RepresentableFieldWidget::_setupSlots() {}

				void RepresentableFieldWidget::localize() {};

				void RepresentableFieldWidget::dragEnterEvent( QDragEnterEvent * event )
				{
					if ( event->mimeData()->hasFormat(
							 UI::MimeType::getQStringMimeType( UI::MimeType::ApplicationMimeType::REPRESENTABLE ) ) )
						event->acceptProposedAction();
				}
				void RepresentableFieldWidget::dropEvent( QDropEvent * event )
				{
					const QByteArray byteData = event->mimeData()->data(
						UI::MimeType::getQStringMimeType( UI::MimeType::ApplicationMimeType::REPRESENTABLE ) );
					const Model::ID idDroppedObject = std::atoi( byteData.data() );

					Model::BaseModel * const representableModel
						= &( MVC::MvcManager::get().getModel<Model::BaseModel>( idDroppedObject ) );

					Generic::BaseRepresentable * representable = nullptr;
					const VTX::ID::VTX_ID &		 modelTypeID   = representableModel->getTypeId();

					if ( modelTypeID == ID::Model::MODEL_MOLECULE )
						representable = static_cast<Generic::BaseRepresentable *>(
							static_cast<Model::Molecule *>( representableModel ) );
					else if ( modelTypeID == ID::Model::MODEL_CHAIN )
						representable = static_cast<Generic::BaseRepresentable *>(
							static_cast<Model::Chain *>( representableModel ) );

					event->acceptProposedAction();

					setValue( representable );
				}
			} // namespace CustomWidget
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
