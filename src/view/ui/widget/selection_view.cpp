#include "selection_view.hpp"
#include "id.hpp"
#include "mvc/mvc_manager.hpp"
#include "style.hpp"
#include "ui/widget_factory.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				SelectionView::SelectionView( Model::Selection * const p_model, QTreeWidgetItem * p_parent ) : BaseSceneItem<Model::Selection>( p_model, p_parent ) {}

				void SelectionView::notify( const Event::VTX_EVENT_MODEL & p_event, const Event::VTXEventModelData * const p_eventData )
				{
					
				}

				void SelectionView::_setupUi( const QString & p_name )
				{
					
				}

				void SelectionView::_setupSlots() {}

				void SelectionView::localize() {}
				
			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX
