#include "ui/old_ui/ui/widget/custom_widget/representable_field_widget.hpp"
#include "ui/old_ui/ui/mime_type.hpp"
#include <app/old/component/chemistry/chain.hpp>
#include <app/old/component/chemistry/molecule.hpp>
#include <app/old/core/model/base_model.hpp>
#include <app/old/id.hpp>
#include <app/old/mvc.hpp>

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

						const bool isRepresentable = ( modelData.getTypeID() == App::Old::ID::Model::MODEL_ATOM )
													 || ( modelData.getTypeID() == App::Old::ID::Model::MODEL_RESIDUE )
													 || ( modelData.getTypeID() == App::Old::ID::Model::MODEL_CHAIN )
													 || ( modelData.getTypeID() == App::Old::ID::Model::MODEL_MOLECULE );

						if ( isRepresentable )
						{
							p_event->acceptProposedAction();
						}
					}
				}
				void RepresentableFieldWidget::dropEvent( QDropEvent * p_event )
				{
					const UI::MimeType::ModelData modelData = UI::MimeType::getModelData( p_event->mimeData() );

					App::Old::Core::Model::BaseModel * const representableModel
						= &( VTX::MVC_MANAGER().getModel<App::Old::Core::Model::BaseModel>( modelData.getModelID() ) );

					App::Old::Application::Representation::BaseRepresentable * representable = nullptr;
					const VTX::App::Old::VTX_ID &							  modelTypeID = representableModel->getTypeId();

					if ( modelTypeID == App::Old::ID::Model::MODEL_MOLECULE )
						representable = static_cast<App::Old::Application::Representation::BaseRepresentable *>(
							static_cast<App::Old::Component::Chemistry::Molecule *>( representableModel ) );
					else if ( modelTypeID == App::Old::ID::Model::MODEL_CHAIN )
						representable = static_cast<App::Old::Application::Representation::BaseRepresentable *>(
							static_cast<App::Old::Component::Chemistry::Chain *>( representableModel ) );

					p_event->acceptProposedAction();

					setValue( representable );
				}
			} // namespace CustomWidget
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
