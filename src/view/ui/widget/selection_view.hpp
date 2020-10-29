#ifndef __VTX_VIEW_UI_WIDGET_SELECTION_VIEW__
#define __VTX_VIEW_UI_WIDGET_SELECTION_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_scene_item.hpp"
#include "model/selection.hpp"
#include "view/base_view.hpp"

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				class SelectionView : public BaseSceneItem<Model::Selection>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					void		 localize() override;
					virtual void notify( const Event::VTX_EVENT_MODEL & p_event, const Event::VTXEventModelData * const p_eventData = 0 ) override;

				  protected:
					SelectionView( Model::Selection * const, QTreeWidgetItem * );
					void _setupUi( const QString & ) override;
					void _setupSlots() override;

				  private:					
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
