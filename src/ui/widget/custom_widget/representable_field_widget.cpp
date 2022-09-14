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

				void RepresentableFieldWidget::dragEnterEvent( QDragEnterEvent * p_event )
				{
					const bool draggedObjectIsModel = UI::MimeType::checkApplicationDataType(
						p_event->mimeData(), UI::MimeType::ApplicationMimeType::MODEL );

					if ( draggedObjectIsModel )
					{
						const UI::MimeType::ModelData modelData = UI::MimeType::getModelData( p_event->mimeData() );

						const bool isRepresentable = ( modelData.getTypeID() == ID::Model::MODEL_ATOM )
													 || ( modelData.getTypeID() == ID::Model::MODEL_RESIDUE )
													 || ( modelData.getTypeID() == ID::Model::MODEL_CHAIN )
													 || ( modelData.getTypeID() == ID::Model::MODEL_MOLECULE );

						if ( isRepresentable )
						{
							p_event->acceptProposedAction();
						}
					}
				}
				void RepresentableFieldWidget::dropEvent( QDropEvent * p_event )
				{
					const UI::MimeType::ModelData modelData = UI::MimeType::getModelData( p_event->mimeData() );

					Model::BaseModel * const representableModel
						= &( MVC::MvcManager::get().getModel<Model::BaseModel>( modelData.getModelID() ) );

					Generic::BaseRepresentable * representable = nullptr;
					const VTX::ID::VTX_ID &		 modelTypeID   = representableModel->getTypeId();

					if ( modelTypeID == VTX::ID::Model::MODEL_MOLECULE )
						representable = static_cast<Generic::BaseRepresentable *>(
							static_cast<Model::Molecule *>( representableModel ) );
					else if ( modelTypeID == VTX::ID::Model::MODEL_CHAIN )
						representable = static_cast<Generic::BaseRepresentable *>(
							static_cast<Model::Chain *>( representableModel ) );

					p_event->acceptProposedAction();

					setValue( representable );
				}
			} // namespace CustomWidget
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
