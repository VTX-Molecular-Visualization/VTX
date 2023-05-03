#include "ui/old_ui/ui/widget/custom_widget/representable_field_widget.hpp"
#include "ui/old_ui/ui/mime_type.hpp"
#include <app/component/chemistry/chain.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/core/model/base_model.hpp>
#include <app/id.hpp>
#include <app/mvc.hpp>

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

						const bool isRepresentable = ( modelData.getTypeID() == App::ID::Model::MODEL_ATOM )
													 || ( modelData.getTypeID() == App::ID::Model::MODEL_RESIDUE )
													 || ( modelData.getTypeID() == App::ID::Model::MODEL_CHAIN )
													 || ( modelData.getTypeID() == App::ID::Model::MODEL_MOLECULE );

						if ( isRepresentable )
						{
							p_event->acceptProposedAction();
						}
					}
				}
				void RepresentableFieldWidget::dropEvent( QDropEvent * p_event )
				{
					const UI::MimeType::ModelData modelData = UI::MimeType::getModelData( p_event->mimeData() );

					App::Core::Model::BaseModel * const representableModel
						= &( VTX::MVC_MANAGER().getModel<App::Core::Model::BaseModel>( modelData.getModelID() ) );

					App::Application::Representation::BaseRepresentable * representable = nullptr;
					const VTX::App::VTX_ID &							  modelTypeID = representableModel->getTypeId();

					if ( modelTypeID == App::ID::Model::MODEL_MOLECULE )
						representable = static_cast<App::Application::Representation::BaseRepresentable *>(
							static_cast<App::Component::Chemistry::Molecule *>( representableModel ) );
					else if ( modelTypeID == App::ID::Model::MODEL_CHAIN )
						representable = static_cast<App::Application::Representation::BaseRepresentable *>(
							static_cast<App::Component::Chemistry::Chain *>( representableModel ) );

					p_event->acceptProposedAction();

					setValue( representable );
				}
			} // namespace CustomWidget
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
